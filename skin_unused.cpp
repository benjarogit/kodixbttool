/**
 * @file skin_unused.cpp
 * @brief Unused texture detection and removal (cross-platform).
 */
#include "skin_unused.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <regex>
#include <set>
#include <fstream>
#include <cstddef>

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#include <windows.h>
#ifndef S_ISDIR
#define S_ISDIR(m) (((m) & _S_IFMT) == _S_IFDIR)
#endif
#ifndef S_ISREG
#define S_ISREG(m) (((m) & _S_IFMT) == _S_IFREG)
#endif
#else
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

static const char *IMAGE_EXTS[] = { ".png", ".jpg", ".jpeg", ".gif", NULL };

static int is_image_ext(const char *name)
{
    size_t len = strlen(name);
    for (int i = 0; IMAGE_EXTS[i] != NULL; i++)
    {
        size_t elen = strlen(IMAGE_EXTS[i]);
        if (len >= elen && strcasecmp(name + len - elen, IMAGE_EXTS[i]) == 0)
            return 1;
    }
    return 0;
}

#ifdef _WIN32
static int strcasecmp(const char *a, const char *b)
{
    return _stricmp(a, b);
}
#endif

std::string skin_unused_default_skin_root()
{
    const char *base = getenv("KODI_ADDONS");
    if (base && base[0])
        return std::string(base) + "/skin.dokukanal";
#ifdef _WIN32
    const char *home = getenv("USERPROFILE");
    if (!home) home = getenv("HOME");
#else
    const char *home = getenv("HOME");
#endif
    if (!home) home = ".";
    return std::string(home) + "/.kodi/addons/skin.dokukanal";
}

#ifdef _WIN32
static void collect_files_recursive_win(const std::string &dir, const char *ext_filter,
                                        std::vector<std::string> &out, const std::string &prefix)
{
    std::string pattern = dir + "\\*";
    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA(pattern.c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return;
    do
    {
        if (strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0)
            continue;
        std::string subpath = prefix.empty() ? fd.cFileName : prefix + "/" + fd.cFileName;
        std::string full = dir + "\\" + fd.cFileName;
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            collect_files_recursive_win(full, ext_filter, out, subpath);
        else if (ext_filter)
        {
            size_t len = strlen(fd.cFileName);
            size_t elen = strlen(ext_filter);
            if (len >= elen && _stricmp(fd.cFileName + len - elen, ext_filter) == 0)
                out.push_back(subpath);
        }
        else if (is_image_ext(fd.cFileName))
            out.push_back(subpath);
    } while (FindNextFileA(h, &fd));
    FindClose(h);
}
#else
static void collect_files_recursive_unix(const char *dir, const char *ext_filter,
                                         std::vector<std::string> &out, const std::string &prefix)
{
    DIR *d = opendir(dir);
    if (!d) return;
    struct dirent *e;
    while ((e = readdir(d)) != nullptr)
    {
        if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0)
            continue;
        std::string subpath = prefix.empty() ? e->d_name : prefix + "/" + e->d_name;
        std::string fullpath = std::string(dir) + "/" + e->d_name;
        struct stat st;
        if (stat(fullpath.c_str(), &st) != 0) continue;
        if (S_ISDIR(st.st_mode))
            collect_files_recursive_unix(fullpath.c_str(), ext_filter, out, subpath);
        else if (ext_filter)
        {
            size_t len = strlen(e->d_name);
            size_t elen = strlen(ext_filter);
            if (len >= elen && strcasecmp(e->d_name + len - elen, ext_filter) == 0)
                out.push_back(subpath);
        }
        else if (S_ISREG(st.st_mode) && is_image_ext(e->d_name))
            out.push_back(subpath);
    }
    closedir(d);
}
#endif

void skin_unused_collect_image_paths(const char *input_dir, std::vector<std::string> &out)
{
#ifdef _WIN32
    collect_files_recursive_win(input_dir, NULL, out, "");
#else
    collect_files_recursive_unix(input_dir, NULL, out, "");
#endif
}

static void collect_xml_refs(const char *filepath, std::vector<std::string> &refs_out)
{
    std::ifstream f(filepath);
    if (!f) return;
    std::string line;
    std::regex re("[\\w./\\-]+\\.(?:png|jpg|jpeg|gif)", std::regex::icase);
    while (std::getline(f, line))
    {
        std::sregex_iterator it(line.begin(), line.end(), re), end;
        for (; it != end; ++it)
            refs_out.push_back(it->str());
    }
}

void skin_unused_collect_refs(const char *skin_root, std::vector<std::string> &refs_out)
{
#ifdef _WIN32
    std::vector<std::string> xmls;
    collect_files_recursive_win(skin_root, ".xml", xmls, "");
    for (const auto &rel : xmls)
    {
        std::string full = std::string(skin_root) + "\\" + rel;
        for (size_t i = 0; i < full.size(); i++)
            if (full[i] == '/') full[i] = '\\';
        collect_xml_refs(full.c_str(), refs_out);
    }
#else
    std::vector<std::string> xmls;
    collect_files_recursive_unix(skin_root, ".xml", xmls, "");
    for (const auto &rel : xmls)
        collect_xml_refs((std::string(skin_root) + "/" + rel).c_str(), refs_out);
#endif
}

void skin_unused_compute_unused(const std::vector<std::string> &texture_paths,
                                const std::vector<std::string> &refs,
                                int exclude_default,
                                std::vector<std::string> &unused_out)
{
    std::set<std::string> ref_set(refs.begin(), refs.end());
    std::set<std::string> ref_basenames;
    for (const auto &r : refs)
    {
        size_t pos = r.find_last_of("/\\");
        ref_basenames.insert(pos == std::string::npos ? r : r.substr(pos + 1));
    }
    for (const auto &path : texture_paths)
    {
        if (exclude_default &&
            (path.compare(0, 7, "default") == 0 || path.compare(0, 7, "Default") == 0))
            continue;
        std::string base = path.substr(path.find_last_of("/\\") + 1);
        if (ref_set.count(path) || ref_basenames.count(base))
            continue;
        unused_out.push_back(path);
    }
    std::sort(unused_out.begin(), unused_out.end());
}

int skin_unused_remove_from_dir(const char *input_dir, const std::vector<std::string> &unused_paths)
{
    int removed = 0;
    for (const auto &rel : unused_paths)
    {
#ifdef _WIN32
        std::string full = std::string(input_dir) + "\\" + rel;
        for (size_t i = strlen(input_dir) + 1; i < full.size(); i++)
            if (full[i] == '/') full[i] = '\\';
#else
        std::string full = std::string(input_dir) + "/" + rel;
#endif
        if (remove(full.c_str()) == 0)
            removed++;
    }
    return removed;
}

bool skin_unused_prompt_remove(size_t unused_count)
{
#ifdef _WIN32
    if (_isatty(_fileno(stdin)) == 0)
        return false;
#else
    if (isatty(STDIN_FILENO) == 0)
        return false;
#endif
    printf("%zu unused texture(s). Remove before pack? (y/N): ", (size_t)unused_count);
    fflush(stdout);
    char buf[32];
    if (!fgets(buf, (int)sizeof(buf), stdin))
        return false;
    char c = buf[0];
    if (c == 'y' || c == 'Y' || c == 'j' || c == 'J')
        return true;
    return false;
}
