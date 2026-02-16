/**
 * @file xbtf_pack.cpp
 * @brief Pack directory of images into Kodi .xbt (Kodi 17+ / Omega 21, LZO-compressed).
 * Part of kodixbttool (C) 2026 Sunny C.
 */
#include "xbtf_pack.h"
#include "xbtf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <vector>
#include <string>
#include <map>

#include <lzo/lzo1x.h>

#ifdef _WIN32
#include <direct.h>
#ifndef strcasecmp
#define strcasecmp _stricmp
#endif
#else
#include <dirent.h>
#include <strings.h>
#include <unistd.h>
#endif

#include <png.h>
#include <jpeglib.h>

static const char VERSION = '2';

#ifndef _WIN32
/** @brief Recursively remove directory and contents. @return 0 on success, -1 on error. */
static int recursive_rmdir(const char *path)
{
    DIR *d = opendir(path);
    if (!d) return -1;
    struct dirent *e;
    int err = 0;
    while ((e = readdir(d)) != nullptr)
    {
        if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0) continue;
        std::string sub = std::string(path) + "/" + e->d_name;
        struct stat st;
        if (stat(sub.c_str(), &st) != 0) { err = -1; continue; }
        if (S_ISDIR(st.st_mode))
        {
            if (recursive_rmdir(sub.c_str()) != 0) err = -1;
        }
        else
        {
            if (unlink(sub.c_str()) != 0) err = -1;
        }
    }
    closedir(d);
    if (rmdir(path) != 0) err = -1;
    return err;
}
#else
/** @brief Stub on Windows: not implemented. */
static int recursive_rmdir(const char *path) { (void)path; return -1; }
#endif

/** @brief Replace backslashes with forward slashes in path. */
static void normalize_path(std::string &path)
{
    for (size_t i = 0; i < path.size(); i++)
        if (path[i] == '\\') path[i] = '/';
}

/** @brief Return non-zero if name has .png, .jpg, .jpeg, or .gif extension (case-insensitive). */
static int is_image_ext(const char *name)
{
    const char *dot = strrchr(name, '.');
    if (!dot) return 0;
    if (strcasecmp(dot, ".png") == 0) return 1;
    if (strcasecmp(dot, ".jpg") == 0) return 1;
    if (strcasecmp(dot, ".jpeg") == 0) return 1;
    if (strcasecmp(dot, ".gif") == 0) return 1;
    return 0;
}

/** @brief Recursively collect relative paths of image files under dir; prefix is current relative path. */
static void collect_images(const char *dir, const std::string &prefix, std::vector<std::string> &out)
{
#ifdef _WIN32
    (void)dir;
    (void)prefix;
    (void)out;
    fprintf(stderr, "Directory scan not implemented for Windows yet.\n");
#else
    DIR *d = opendir(dir);
    if (!d) return;
    struct dirent *e;
    while ((e = readdir(d)) != nullptr)
    {
        if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0)
            continue;
        std::string subpath = prefix.empty() ? e->d_name : prefix + "/" + e->d_name;
        std::string fullpath = std::string(dir) + "/" + e->d_name;  /* path for stat/open */
        struct stat st;
        if (stat(fullpath.c_str(), &st) != 0) continue;
        if (S_ISDIR(st.st_mode))
        {
            collect_images(fullpath.c_str(), subpath, out);
        }
        else if (S_ISREG(st.st_mode) && is_image_ext(e->d_name))
        {
            out.push_back(subpath);
        }
    }
    closedir(d);
#endif
}

/** @brief Load PNG into RGBA/BGRA buffer; w, h, has_alpha set. Caller must free. */
static unsigned char *load_png(const char *path, unsigned int *w, unsigned int *h, int *has_alpha)
{
    FILE *f = fopen(path, "rb");
    if (!f) return nullptr;
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png) { fclose(f); return nullptr; }
    png_infop info = png_create_info_struct(png);
    if (!info) { png_destroy_read_struct(&png, nullptr, nullptr); fclose(f); return nullptr; }
    if (setjmp(png_jmpbuf(png))) { png_destroy_read_struct(&png, &info, nullptr); fclose(f); return nullptr; }
    png_init_io(png, f);
    png_read_info(png, info);
    *w = png_get_image_width(png, info);
    *h = png_get_image_height(png, info);
    int color_type = png_get_color_type(png, info);
    *has_alpha = (color_type & PNG_COLOR_MASK_ALPHA) ? 1 : 0;
    png_set_expand(png);
    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);
    if (!(*has_alpha) && color_type == PNG_COLOR_TYPE_GRAY)
        png_set_gray_to_rgb(png);
    if (png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);
    png_read_update_info(png, info);
    size_t rowbytes = png_get_rowbytes(png, info);
    size_t total = rowbytes * (*h);
    unsigned char *buf = (unsigned char *)malloc(total);
    if (!buf) { png_destroy_read_struct(&png, &info, nullptr); fclose(f); return nullptr; }
    png_bytep *rows = (png_bytep *)malloc(sizeof(png_bytep) * (*h));
    if (!rows) { free(buf); png_destroy_read_struct(&png, &info, nullptr); fclose(f); return nullptr; }
    for (unsigned int y = 0; y < *h; y++)
        rows[y] = buf + y * rowbytes;
    png_read_image(png, rows);
    free(rows);
    png_destroy_read_struct(&png, &info, nullptr);
    fclose(f);
    /* Convert RGBA to BGRA (A8R8G8B8) for Kodi */
    if (*has_alpha)
    {
        for (size_t i = 0; i < total; i += 4)
        { unsigned char t = buf[i]; buf[i] = buf[i+2]; buf[i+2] = t; }
    }
    return buf;
}

/** @brief Load JPEG into RGB buffer; w, h set; has_alpha set to 0. Caller must free. */
static unsigned char *load_jpeg(const char *path, unsigned int *w, unsigned int *h, int *has_alpha)
{
    *has_alpha = 0;
    FILE *f = fopen(path, "rb");
    if (!f) return nullptr;
    struct jpeg_decompress_struct jpeg;
    struct jpeg_error_mgr jerr;
    jpeg.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&jpeg);
    jpeg_stdio_src(&jpeg, f);
    jpeg_read_header(&jpeg, TRUE);
    jpeg.out_color_space = JCS_EXT_RGB;
    jpeg.output_components = 3;
    jpeg_start_decompress(&jpeg);
    *w = jpeg.output_width;
    *h = jpeg.output_height;
    size_t rowbytes = *w * 3;
    size_t total = rowbytes * (*h);
    unsigned char *buf = (unsigned char *)malloc(total);
    if (!buf) { jpeg_destroy_decompress(&jpeg); fclose(f); return nullptr; }
    while (jpeg.output_scanline < jpeg.output_height)
    {
        unsigned char *row = buf + jpeg.output_scanline * rowbytes;
        jpeg_read_scanlines(&jpeg, &row, 1);
    }
    jpeg_finish_decompress(&jpeg);
    jpeg_destroy_decompress(&jpeg);
    fclose(f);
    return buf;
}

/** @brief Load image by extension (PNG/JPEG); set format to XB_FMT_A8R8G8B8 or XB_FMT_RGB8. Caller must free. */
static unsigned char *load_image(const char *path, unsigned int *w, unsigned int *h, int *has_alpha, uint32_t *format)
{
    const char *dot = strrchr(path, '.');
    if (!dot) return nullptr;
    unsigned char *buf = nullptr;
    if (strcasecmp(dot, ".png") == 0)
    {
        buf = load_png(path, w, h, has_alpha);
        *format = (*has_alpha) ? XB_FMT_A8R8G8B8 : XB_FMT_RGB8;
    }
    else if (strcasecmp(dot, ".jpg") == 0 || strcasecmp(dot, ".jpeg") == 0)
    {
        buf = load_jpeg(path, w, h, has_alpha);
        *format = XB_FMT_RGB8;
    }
    else if (strcasecmp(dot, ".gif") == 0)
    {
        /* GIF: skip for v1 or add single-frame decode later */
        fprintf(stderr, "GIF pack not yet implemented, skipping: %s\n", path);
        return nullptr;
    }
    return buf;
}

/** Raw pixel blob for dupecheck. */
struct Blob { std::vector<unsigned char> data; };
struct FileEntry {
    std::string path;
    uint32_t width, height, format;
    uint64_t packed;
    size_t blob_index;
};

/** @brief Write 32-bit value little-endian. @return 0 on success. */
static int write_u32(FILE *f, uint32_t v)
{
    return fwrite(&v, 4, 1, f) == 1 ? 0 : -1;
}
/** @brief Write 64-bit value little-endian. @return 0 on success. */
static int write_u64(FILE *f, uint64_t v)
{
    return fwrite(&v, 8, 1, f) == 1 ? 0 : -1;
}
/** @brief Write size bytes. @return 0 on success. */
static int write_str(FILE *f, const char *buf, size_t size)
{
    return fwrite(buf, 1, size, f) == size ? 0 : -1;
}

/** @brief Pack directory of images into .xbt (XBT v2, A8R8G8B8/RGB8). See xbtf_pack.h. */
int xbtf_pack(const char *input_dir, const char *output_path, int dupecheck, int remove_input)
{
    std::vector<std::string> paths;
    collect_images(input_dir, "", paths);
    if (paths.empty())
    {
        fprintf(stderr, "No image files found in %s\n", input_dir);
        return -1;
    }

    std::vector<Blob> blobs;
    std::vector<FileEntry> entries;
    std::map<size_t, size_t> hash_to_blob; /* simple hash -> blob index for dupecheck */

    for (size_t i = 0; i < paths.size(); i++)
    {
        std::string fullpath = std::string(input_dir) + "/" + paths[i];
        normalize_path(fullpath);
        unsigned int w, h;
        int has_alpha;
        uint32_t format;
        unsigned char *pixels = load_image(fullpath.c_str(), &w, &h, &has_alpha, &format);
        if (!pixels) continue;

        size_t size = (format == XB_FMT_RGB8) ? (w * h * 3) : (w * h * 4);
        size_t blob_index = blobs.size();
        if (dupecheck)
        {
            size_t hash = size + w + (size >> 4);
            for (size_t k = 0; k < size && k < 1024; k++) hash = hash * 31 + pixels[k];
            auto it = hash_to_blob.find(hash);
            if (it != hash_to_blob.end())
            {
                if (blobs[it->second].data.size() == size && memcmp(blobs[it->second].data.data(), pixels, size) == 0)
                    blob_index = it->second;
            }
            if (blob_index == blobs.size())
                hash_to_blob[hash] = blobs.size();
        }
        if (blob_index == blobs.size())
        {
            Blob b;
            b.data.assign(pixels, pixels + size);
            blobs.push_back(std::move(b));
        }
        free(pixels);

        FileEntry e;
        e.path = paths[i];
        normalize_path(e.path);
        e.width = w;
        e.height = h;
        e.format = format;
        e.packed = size;
        e.blob_index = blob_index;
        entries.push_back(e);
    }

    if (entries.empty())
    {
        fprintf(stderr, "No images could be loaded.\n");
        return -1;
    }

    if (lzo_init() != LZO_E_OK)
    {
        fprintf(stderr, "LZO init failed\n");
        return -1;
    }

    /* LZO-compress each blob; if not smaller, store uncompressed (packed == unpacked). */
    std::vector<std::vector<unsigned char>> compressed_blobs(blobs.size());
    lzo_voidp wrkmem = malloc(LZO1X_1_MEM_COMPRESS);
    if (!wrkmem) { fprintf(stderr, "Out of memory (LZO wrkmem)\n"); return -1; }
    for (size_t i = 0; i < blobs.size(); i++)
    {
        const std::vector<unsigned char> &raw = blobs[i].data;
        lzo_uint in_len = (lzo_uint)raw.size();
        lzo_uint out_len_max = in_len + in_len / 16 + 64 + 3;
        std::vector<unsigned char> cbuf(out_len_max);
        lzo_uint out_len = 0;
        int r = lzo1x_1_compress(raw.data(), in_len, cbuf.data(), &out_len, wrkmem);
        if (r != LZO_E_OK || out_len >= in_len)
        {
            /* store uncompressed */
            compressed_blobs[i] = raw;
        }
        else
        {
            cbuf.resize(out_len);
            compressed_blobs[i] = std::move(cbuf);
        }
    }
    free(wrkmem);

    /* Compute offsets: header then blobs in order (compressed). */
    std::vector<uint64_t> blob_offset(blobs.size(), 0);
    size_t header_size = 4 + 1 + 4; /* magic + version + numFiles */
    header_size += entries.size() * (MAX_STR + 4 + 4 + (4+4+4+8+8+4+8)); /* fullPath, loop, numFrames, one frame meta */
    uint64_t pos = header_size;
    for (size_t i = 0; i < blobs.size(); i++)
    {
        blob_offset[i] = pos;
        pos += compressed_blobs[i].size();
    }

    FILE *out = fopen(output_path, "wb");
    if (!out)
    {
        fprintf(stderr, "Cannot create output: %s\n", output_path);
        return -1;
    }

    if (write_str(out, MAGIC, 4) != 0 || fwrite(&VERSION, 1, 1, out) != 1 || write_u32(out, (uint32_t)entries.size()) != 0)
    {
        fclose(out);
        return -1;
    }

    for (size_t i = 0; i < entries.size(); i++)
    {
        const FileEntry &e = entries[i];
        char fullPath[MAX_STR];
        memset(fullPath, 0, MAX_STR);
        size_t len = e.path.size();
        if (len >= MAX_STR) len = MAX_STR - 1;
        memcpy(fullPath, e.path.c_str(), len);
        if (write_str(out, fullPath, MAX_STR) != 0 || write_u32(out, 0) != 0 || write_u32(out, 1) != 0)
            goto fail;
        uint64_t offset = blob_offset[e.blob_index];
        uint64_t unpacked = e.packed;
        uint64_t packed = (uint64_t)compressed_blobs[e.blob_index].size();
        if (write_u32(out, e.width) != 0 || write_u32(out, e.height) != 0 || write_u32(out, e.format) != 0 ||
            write_u64(out, packed) != 0 || write_u64(out, unpacked) != 0 || write_u32(out, 0) != 0 || write_u64(out, offset) != 0)
            goto fail;
    }

    for (size_t i = 0; i < blobs.size(); i++)
    {
        const std::vector<unsigned char> &cb = compressed_blobs[i];
        if (fwrite(cb.data(), 1, cb.size(), out) != cb.size())
            goto fail;
    }

    fclose(out);

    if (remove_input)
    {
#ifdef _WIN32
        fprintf(stderr, "remove-input not implemented on Windows yet.\n");
#else
        fprintf(stderr, "Removing input directory: %s\n", input_dir);
        recursive_rmdir(input_dir);
#endif
    }
    return 0;
fail:
    fclose(out);
    remove(output_path);
    return -1;
}
