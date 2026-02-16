/*
 * Based on xbtfextractor (C) 2012 Lars Hall.
 * This variant kodixbttool (C) 2026 Sunny C.
 *
 * kodixbttool is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * kodixbttool is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with kodixbttool.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "xbtf.h"
#include "platform.h"
#ifdef _WIN32
#include <direct.h>
#include <sys/stat.h>
#else
#include <sys/stat.h>
#endif

/** @brief Initialize LZO; sets fileOpened to false. */
Xbtf::Xbtf()
{
    fileOpened = false;
    if (lzo_init() != LZO_E_OK)
        fprintf(stderr, "Cannot initialize lzo\n");
}

/** @brief Open .xbt and read header. @return true on success. */
bool Xbtf::open(const char *filename)
{
    file = fopen (filename, "rb");
    if (file == NULL)
    {
        fprintf(stderr, "Cannot open file:%s\n", filename);
        return false;
    }

    fileOpened = true;

    return readHeader();
}

/** @brief Parse full file list and frame metadata. @return true on success. */
bool Xbtf::parse()
{
    return readMeta();
}

/** @brief Read XBTF magic and version. @return true if valid. */
bool Xbtf::readHeader()
{
    char magic[4];
    readStr(magic, 4);

    if (strncmp(magic, MAGIC, sizeof(magic)) != 0)
    {
        fprintf(stderr, "Wrong filetype\n");
        return false;
    }

    readStr(version, 1);
    version[1] = '\0';

    return true;
}

/** @brief Read file count and all MediaFile/frame metadata. @return true on success. */
bool Xbtf::readMeta()
{
    uint32_t numFiles = readU32();
    for (uint32_t i = 0; i < numFiles; i++)
    {
        MediaFile mediaFile;
        memset(mediaFile.fullPath, 0, sizeof(mediaFile.fullPath));
        readStr(mediaFile.fullPath, sizeof(mediaFile.fullPath));

        mediaFile.loop = readU32();
        mediaFile.numFrames = readU32();

        // parse path/filename/ext
        parsePath(mediaFile);

        for (uint32_t j = 0; j < mediaFile.numFrames; j++)
        {
            Frame frame;
            // width
            frame.width = readU32();
            // height
            frame.height = readU32();
            //format
            frame.format = readU32();
            // packed size
            frame.packed = readU64();
            // unpacked size
            frame.unpacked = readU64();
            // duration
            frame.duration = readU32();
            // offset
            frame.offset = readU64();

            mediaFile.frames.push_back(frame);
        }

        char *key = new char[MAX_STR];
        strncpy(key, mediaFile.fullPath, MAX_STR);
        mediaFiles.insert(make_pair(key, mediaFile));
    }

    return true;
}

/** @brief Read size bytes from file into buf. */
void Xbtf::readStr(char *buf, size_t size)
{
    fread(buf, size, 1, file);
}

/** @brief Read little-endian 32-bit value. */
uint32_t Xbtf::readU32()
{
    uint64_t ret;
    fread(&ret, 4, 1, file);
    return ret;
}

/** @brief Read little-endian 64-bit value. */
uint64_t Xbtf::readU64()
{
    uint64_t ret;
    fread(&ret, 8, 1, file);
    return ret;
}

/** @brief Split fullPath into path, filename, and set fileType from extension. */
void Xbtf::parsePath(MediaFile &mediaFile)
{
    int fullLen = strnlen(mediaFile.fullPath, sizeof(mediaFile.fullPath));

    char tmp[MAX_STR];
    memset(tmp, 0, MAX_STR);

    int i = 0;
    int pathSep = 0;
    int extSep = 0;

    for (i = fullLen; i > -1; i--)
    {
        // find last occurence of '.ext' in the same run
        // as finding the last occurence of '/'
        if ((mediaFile.fullPath[i] == '.') && (extSep == 0))
            extSep = i;
        if (mediaFile.fullPath[i] == '/')
        {
            pathSep = i;
            break;
        }
    }
   
    for (i = 0; i < pathSep + 1; i++)
        tmp[i] = mediaFile.fullPath[i];

    strncpy(mediaFile.path, tmp, sizeof(tmp));

    char *filename = mediaFile.fullPath;
    filename += pathSep + 1;
    strncpy(mediaFile.filename, filename, sizeof(tmp)); 

    char *extension = mediaFile.fullPath;
    extension += extSep + 1;
    
    if (strncmp(extension, "png", 3) == 0)
        mediaFile.fileType = MediaFile::PNG;
    else if (strncmp(extension, "jpg", 3) == 0)
        mediaFile.fileType = MediaFile::JPEG;
    else if (strncmp(extension, "jpeg", 4) == 0)
        mediaFile.fileType = MediaFile::JPEG;
    else if (strncmp(extension, "gif", 3) == 0)
        mediaFile.fileType = MediaFile::GIF;
}

/** @brief Create directory path and parents; use cache to avoid repeated stat. @return true on success. */
bool Xbtf::createPath(const char *path)
{
    char *pp, *sp;
    int status = 0;
    char tmpPath[MAX_STR];

    // if for any reason the path contains ".."  don't do anything
    if(strstr(path, "..") != NULL)
        return false;

    // Don't want to hit disk every time to see if path
    // exist. If path exist in cache, just return otherwise
    // add it to cache and create the dir
    if (isPathInCache(path))
        return true;
    else
    {
        char *tmp = new char[MAX_STR];
        strncpy(tmp, path, MAX_STR);
        pathCache.insert(make_pair(tmp, true));
    }

    strncpy(tmpPath, path, MAX_STR);
    pp = tmpPath;

    while (status == 0 && (sp = strchr(pp, '/')) != 0)
    {
        if (sp != pp)
        {
            *sp = '\0';
            status = makeDir(tmpPath);
            *sp = '/';
        }
        pp = sp + 1;
    }
    /* create final segment (e.g. out_test/common when path has no further slash) */
    if (status == 0 && *pp)
        status = makeDir(tmpPath);

    return status == 0;
}

/** @brief Create single directory with common permissions. @return 0 on success. */
int Xbtf::makeDir(const char *dir)
{
#ifdef _WIN32
    struct _stat info;
    int status = 0;
    if (_stat(dir, &info) != 0)
    {
        if (_mkdir(dir) != 0)
            status = -1;
    }
    return status;
#else
    struct stat info;
    int status = 0;
    if (stat(dir, &info) != 0)
    {
        if (mkdir(dir, S_IRWXU | S_IXGRP | S_IRGRP | S_IROTH | S_IXOTH) != 0)
            status = -1;
    }
    return status;
#endif
}

/** @brief Check if path was already created (cached). */
bool Xbtf::isPathInCache(const char *path)
{
    PathCache::const_iterator it;
    it = pathCache.find(path);
    return it != pathCache.end();
}

/** @brief Decompress one frame (LZO or copy) into dst. @return true on success. */
bool Xbtf::uncompressFrame(unsigned char *dst, const Frame &frame)
{
    bool ret = true;
    unsigned char *packed = new unsigned char[frame.packed];
    memset(packed, 0, frame.packed);

    if (xbt_fseek64(file, (int64_t)frame.offset, SEEK_SET) == -1)
    {
        fprintf(stderr, "Cannot seek to location\n");
        ret = false;;
    }
    else
    {
        fread(packed, frame.packed, 1, file);

        if (frame.packed != frame.unpacked)
        {
            lzo_uint len = (lzo_uint)frame.unpacked;
            if ((lzo1x_decompress(packed, (lzo_uint)frame.packed,
                dst, &len, NULL) != LZO_E_OK) || (len != frame.unpacked))
            {
                ret = false;
            }
        }
        else
            memcpy(dst, packed, frame.packed);
    }

    delete [] packed;

    return ret;
}

/** @brief Decompress DXT1/DXT3/DXT5 frame in-place. */
bool Xbtf::decompressDXT(unsigned char *data,
    unsigned int len, const Frame &frame)
{
    squish::u8 *buf = new squish::u8[len];
    memset(buf, 0, len);

    switch (frame.format)
    {
        case XB_FMT_DXT1:
            squish::DecompressImage(buf, frame.width,
                frame.height, data, squish::kDxt1);
            break;
        case XB_FMT_DXT3:
            squish::DecompressImage(buf, frame.width,
                frame.height, data, squish::kDxt3);
            break;
        case XB_FMT_DXT5:
            squish::DecompressImage(buf, frame.width,
                frame.height, data, squish::kDxt5);
            break;
        default:
            fprintf(stderr, "Wrong DXT format\n");
    }

    memcpy(data, buf, len);
    delete [] buf;

    return true;
}

/** @brief Extract every file to dst. */
void Xbtf::extractAllFiles(const char *dst, bool createDirs)
{
    MediaFiles::iterator it;
    for (it = mediaFiles.begin(); it != mediaFiles.end(); it++)
        extractMediaFile(it->second, dst, createDirs);
}

/** @brief Extract one file by path inside the .xbt. */
void Xbtf::extractFile(const char *filename, const char *dst, bool createDirs)
{
    MediaFiles::iterator it;
    it = mediaFiles.find((char*)filename);
    if (it != mediaFiles.end())
        extractMediaFile(it->second, dst, createDirs);
    else
        fprintf(stderr, "Cannot extract file: %s. File not found", filename);
}

/** @brief Extract one media file (all frames) to dst; create dirs if createDirs. Writes PNG/JPEG/GIF. */
void Xbtf::extractMediaFile(MediaFile &mediaFile,
    const char *dst, bool createDirs)
{
    parsePath(mediaFile);

    for (unsigned int i = 0; i < mediaFile.frames.size(); i++)
    {
        unsigned int len = 0;
        if (mediaFile.frames[i].format & XB_FMT_DXT_MASK)
            len = mediaFile.frames[i].width * mediaFile.frames[i].height * 10;
        else
            len = mediaFile.frames[i].unpacked;

        fprintf(stderr, "Extracting file: %s\n", mediaFile.fullPath);
        unsigned char *data = new unsigned char[len];
        uncompressFrame(data, mediaFile.frames[i]);

        if (mediaFile.frames[i].format & XB_FMT_DXT_MASK)
        {
            decompressDXT(data, len, mediaFile.frames[i]);
            mediaFile.frames[i].unpacked = len;
        }

        char dstPath[MAX_STR * 2];
        size_t dlen = strnlen(dst, MAX_STR * 2 - 1);
        if (dlen >= MAX_STR * 2) dlen = MAX_STR * 2 - 1;
        strncpy(dstPath, dst, MAX_STR * 2);
        dstPath[MAX_STR * 2 - 1] = '\0';
        if (dlen > 0 && dstPath[dlen - 1] != '/')
        {
            dstPath[dlen] = '/';
            dstPath[dlen + 1] = '\0';
            dlen++;
        }
        size_t pathRoom = (MAX_STR * 2) - dlen - 1;
        strncat(dstPath, mediaFile.path, pathRoom);
        dstPath[MAX_STR * 2 - 1] = '\0';
        dlen = strnlen(dstPath, MAX_STR * 2);
        pathRoom = (MAX_STR * 2) - dlen - 1;
        strncat(dstPath, mediaFile.filename, pathRoom);

        if (createDirs)
        {
            char dirPath[MAX_STR * 2];
            strncpy(dirPath, dstPath, MAX_STR * 2 - 1);
            dirPath[MAX_STR * 2 - 1] = '\0';
            char *lastSlash = strrchr(dirPath, '/');
            if (lastSlash && lastSlash > dirPath)
            {
                *lastSlash = '\0';
                if (!createPath(dirPath))
                {
                    fprintf(stderr, "Cannot create dir:%s\n", dirPath);
                    delete [] data;
                    break;
                }
            }
        }

        switch(mediaFile.fileType)
        {
            case MediaFile::PNG:
                compressPng(mediaFile.frames[i], data, dstPath);
                break;
            case MediaFile::JPEG:
                compressJpeg(mediaFile.frames[i], data, dstPath);
                break;
            case MediaFile::GIF:
                compressGif(mediaFile.frames[i], data, dstPath);
                break;
        }

        delete [] data;
    }
}

/** @brief Compute row stride and hasAlpha from frame format (for PNG/JPEG export). */
void Xbtf::getStrideAndAlpha(const Frame &frame, unsigned int &stride, bool &hasAlpha) const
{
    hasAlpha = (frame.format != XB_FMT_RGB8);
    stride = frame.width * (hasAlpha ? 4 : 3);
}

/** @brief Write frame as JPEG to filename. @return true on success. */
bool Xbtf::compressJpeg(const Frame &frame, unsigned char *data,
    const char *filename)
{
    unsigned int stride;
    bool hasAlpha;
    getStrideAndAlpha(frame, stride, hasAlpha);

    // FIXME: Check that this can be open and written to
    FILE *jpegFile = fopen(filename, "wb");

    if (jpegFile == NULL)
    {
        fprintf(stderr, "Cannot create jpeg file: %s\n", filename);
        return false;
    }

    struct jpeg_compress_struct jpeg;
    struct jpeg_error_mgr jerr;
     
    jpeg.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&jpeg);
    jpeg_stdio_dest(&jpeg, jpegFile);
     
    jpeg.image_width = frame.width;
    jpeg.image_height = frame.height;

    if (!hasAlpha)
    {
        jpeg.input_components = 3;
        jpeg.in_color_space = JCS_EXT_RGB;
    }
    else
    {
        jpeg.input_components = 4;
        jpeg.in_color_space = JCS_EXT_RGBA;
    }

    if (frame.format == XB_FMT_A8R8G8B8)
    {
        if (hasAlpha)
            jpeg.in_color_space = JCS_EXT_BGRA;
        else
            jpeg.in_color_space = JCS_EXT_BGR;
    }

    jpeg_set_defaults(&jpeg);
    jpeg_set_quality (&jpeg, 100, true);
    jpeg_start_compress(&jpeg, true);

    JSAMPROW row;
 
    while (jpeg.next_scanline < jpeg.image_height)
    {
        row = (JSAMPROW) &data[jpeg.next_scanline * stride];
        jpeg_write_scanlines(&jpeg, &row, 1);
    }

    jpeg_finish_compress(&jpeg);
    fclose(jpegFile);

    return true;
}

/** @brief Write frame as PNG to filename. @return true on success. */
bool Xbtf::compressPng(const Frame &frame, unsigned char *data,
    const char *filename)
{
    unsigned int stride;
    bool hasAlpha;
    getStrideAndAlpha(frame, stride, hasAlpha);

    FILE *pngFile = fopen(filename, "wb");
    if (!pngFile)
    {
        fprintf(stderr, "Cannot create png file: %s\n", filename);
        return false;
    }

    png_structp png;
    png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png)
    {
        fprintf(stderr, "Cannot create struct\n");
        return false;
    }

    png_infop pngInfo = png_create_info_struct(png);
    if (!pngInfo)
    {
        fprintf(stderr, "Cannot create struct\n");
        return false;
    }

    if (setjmp(png_jmpbuf(png)))
        fprintf(stderr, "setjmp failed\n");

    png_init_io(png, pngFile);

    // write header
    if (setjmp(png_jmpbuf(png)))
    {
        fprintf(stderr, "setjmp failed\n");
        return false;
    }

    png_byte colorType;
    if (!hasAlpha)
        colorType = PNG_COLOR_TYPE_RGB;
    else
        colorType = PNG_COLOR_TYPE_RGBA;

    // convert BGRA if necessary
    if (frame.format == XB_FMT_A8R8G8B8)
         png_set_bgr(png);
 
    png_byte bitDepth = 8;

    png_set_IHDR(png, pngInfo, frame.width, frame.height,
        bitDepth, colorType, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png, pngInfo);

    if (setjmp(png_jmpbuf(png)))
    {
        fprintf(stderr, "setjmp failed\n");
        return false;
    }

    png_byte *row;

    // Write image data
    for (unsigned int y = 0; y < frame.height; y++)
    {
        row = &data[y * stride];
        png_write_row(png, row);
    }


    if (setjmp(png_jmpbuf(png)))
    {
        fprintf(stderr, "end of file (setjmp) failed.\n");
        return false;
    }

    png_write_end(png, NULL);

    fclose(pngFile);

    return true;
}

/** @brief Stub: GIF extraction not supported with giflib 5.x. Always returns false. */
bool Xbtf::compressGif(const Frame &frame, unsigned char *data,
    const char *filename)
{
    (void)frame;
    (void)data;
    fprintf(stderr, "GIF extraction not supported with giflib 5.x (skipped): %s\n", filename);
    return false;
}

/** @brief Print all fullPath entries to stdout. */
void Xbtf::printFiles()
{
    MediaFiles::iterator it;
    for (it = mediaFiles.begin(); it != mediaFiles.end(); it++)
        fprintf(stdout, "File:%s\n", it->second.fullPath);
}

Xbtf::~Xbtf()
{
    MediaFiles::iterator i;
    for (i = mediaFiles.begin(); i != mediaFiles.end(); i++)
        delete [] i->first;

    PathCache::iterator j;
    for (j = pathCache.begin(); j != pathCache.end(); j++)
        delete [] j->first;

    if (fileOpened)
        fclose(file);
}
