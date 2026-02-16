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

#include <stdio.h>
#ifdef _WIN32
#include "getopt_win.h"
#else
#include <getopt.h>
#endif
#include <string>
#include <vector>
#include <sys/stat.h>
#include "xbtf.h"
#include "xbtf_pack.h"
#include "skin_unused.h"

#ifdef _WIN32
static int path_is_dir(const char *path)
{
    struct _stat st;
    return _stat(path, &st) == 0 && (st.st_mode & _S_IFMT) == _S_IFDIR;
}
#else
static int path_is_dir(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}
#endif

/** @brief Print usage and options to stdout, then exit(0) is not called here. */
void printHelp()
{
    fprintf(stdout, "Usage: kodixbttool [OPTION...] [FILE]                 \n");
    fprintf(stdout, "       kodixbttool --pack -i INPUT_DIR -o OUTPUT.xbt [options]\n");
    fprintf(stdout, "       kodixbttool --list-unused -i INPUT_DIR [--skin-dir=DIR]\n\n");
    fprintf(stdout, "Examples:                                             \n");
    fprintf(stdout, "  kodixbttool -o ./dir/ -c textures.xbt    # Extracts textures.xbt to dir\n");
    fprintf(stdout, "  kodixbttool -p xbtfile.xbt               # Prints all paths in the xbt\n");
    fprintf(stdout, "  kodixbttool --list-unused -i ./Textures_extracted  # List unused textures (skin from KODI_ADDONS or $HOME/.kodi/addons)\n");
    fprintf(stdout, "  kodixbttool --pack -i ./Textures_extracted -o ./Textures.xbt  # Pack; prompts to remove unused if skin-dir set\n\n");
    fprintf(stdout, "Options (extract):                                     \n");
    fprintf(stdout, "  -o, --outdir=OUTDIR   Output directory for extraction\n");
    fprintf(stdout, "  -c, --create          Create directory tree          \n");
    fprintf(stdout, "  -f, --file=FILENAME   Extract only FILENAME          \n");
    fprintf(stdout, "  -p, --print           Print all paths in xbt        \n");
    fprintf(stdout, "Options (pack):                                        \n");
    fprintf(stdout, "  --pack                Pack directory to .xbt        \n");
    fprintf(stdout, "  -i, --input=DIR       Input directory (images)      \n");
    fprintf(stdout, "  --skin-dir=DIR        Skin root for unused check (default: KODI_ADDONS or $HOME/.kodi/addons/skin.dokukanal)\n");
    fprintf(stdout, "  --remove-unused       Remove unused textures before pack (no prompt)\n");
    fprintf(stdout, "  --no-remove-unused    Do not remove unused textures (no prompt)\n");
    fprintf(stdout, "  --dupecheck           Store identical images once   \n");
    fprintf(stdout, "  --no-remove-input     Do not delete input dir after pack (default: delete)\n");
    fprintf(stdout, "  --list-unused         List textures not referenced in skin XMLs (use with -i INPUT_DIR)\n");
    fprintf(stdout, "  -h, --help            Print this help                \n\n");
}

/**
 * @brief Entry point: extract, pack, or list .xbt per command-line options.
 * @param argc Argument count
 * @param argv Argument vector (options and file/dir paths)
 * @return 0 on success, 1 on error (or exit(1) on invalid usage)
 */
int main(int argc, char **argv)
{
    bool createDirs = false;
    int print = 0;
    int packMode = 0;
    int dupecheck = 0;
    int removeInput = 1;  /* default: remove input dir after pack */
    int listUnused = 0;
    int removeUnused = -1;  /* -1 = prompt if TTY, 0 = no, 1 = yes */
    char *skinDir = NULL;

    char *outDir = NULL;
    char *xbtfFile = NULL;
    char *filename = NULL;
    char *packInput = NULL;
    char *packOutput = NULL;

    static struct option long_options[] =
    {
        {"help",             no_argument,       0, 'h'},
        {"print",            no_argument,       0, 'p'},
        {"create-dirs",      no_argument,       0, 'c'},
        {"outdir",           required_argument, 0, 'o'},
        {"file",             required_argument, 0, 'f'},
        {"pack",             no_argument,       0, 200},
        {"input",            required_argument, 0, 'i'},
        {"skin-dir",         required_argument, 0, 204},
        {"remove-unused",    no_argument,       0, 205},
        {"no-remove-unused", no_argument,       0, 206},
        {"list-unused",      no_argument,       0, 207},
        {"dupecheck",        no_argument,       0, 201},
        {"remove-input",    no_argument,       0, 202},
        {"no-remove-input", no_argument,       0, 203},
        {0, 0, 0, 0}
    };

    while (true)
    {
        int option_index = 0;
        int c = getopt_long(argc, argv, "hpcf:o:i:", long_options, &option_index);
        if (c == -1) break;
        switch (c)
        {
            case 'h':
                printHelp();
                exit(0);
            case 'o':
                outDir = optarg;
                break;
            case 'c':
                createDirs = true;
                break;
            case 'f':
                filename = optarg;
                break;
            case 'p':
                print = true;
                break;
            case 'i':
                packInput = optarg;
                break;
            case 200:
                packMode = 1;
                break;
            case 201:
                dupecheck = 1;
                break;
            case 202:
                removeInput = 1;
                break;
            case 203:
                removeInput = 0;
                break;
            case 204:
                skinDir = optarg;
                break;
            case 205:
                removeUnused = 1;
                break;
            case 206:
                removeUnused = 0;
                break;
            case 207:
                listUnused = 1;
                break;
            default:
                break;
        }
    }
    if (optind < argc)
    {
        if (!packMode)
            xbtfFile = argv[optind++];
        else if (!packOutput)
            packOutput = argv[optind++];
    }

    if (listUnused)
    {
        if (!packInput)
        {
            fprintf(stderr, "list-unused requires -i INPUT_DIR\n");
            printHelp();
            exit(1);
        }
        std::string skin_root = skinDir ? skinDir : skin_unused_default_skin_root();
        std::vector<std::string> paths, refs, unused;
        skin_unused_collect_image_paths(packInput, paths);
        skin_unused_collect_refs(skin_root.c_str(), refs);
        skin_unused_compute_unused(paths, refs, 1, unused);
        fprintf(stdout, "Textures: %zu  Refs: %zu  Unused (excl. default*): %zu\n",
                (size_t)paths.size(), (size_t)refs.size(), (size_t)unused.size());
        for (const auto &p : unused)
            fprintf(stdout, "%s\n", p.c_str());
        return 0;
    }

    if (packMode)
    {
        if (outDir) packOutput = outDir;
        if (!packInput || !packOutput)
        {
            fprintf(stderr, "Pack mode requires -i INPUT_DIR and -o OUTPUT.xbt\n");
            printHelp();
            exit(1);
        }
        std::string skin_root = skinDir ? skinDir : skin_unused_default_skin_root();
        bool have_skin = path_is_dir(skin_root.c_str());
        if (have_skin)
        {
            std::vector<std::string> paths, refs, unused;
            skin_unused_collect_image_paths(packInput, paths);
            skin_unused_collect_refs(skin_root.c_str(), refs);
            skin_unused_compute_unused(paths, refs, 1, unused);
            if (!unused.empty())
            {
                int do_remove = 0;
                if (removeUnused == 1)
                    do_remove = 1;
                else if (removeUnused == 0)
                    do_remove = 0;
                else
                    do_remove = skin_unused_prompt_remove(unused.size()) ? 1 : 0;
                if (do_remove)
                {
                    int n = skin_unused_remove_from_dir(packInput, unused);
                    fprintf(stderr, "Removed %d unused texture(s).\n", n);
                }
            }
        }
        if (xbtf_pack(packInput, packOutput, dupecheck, removeInput) != 0)
            exit(1);
        /* Remove .bak next to output if present (cleanup after pack). */
        std::string bakPath = std::string(packOutput) + ".bak";
        remove(bakPath.c_str());
        return 0;
    }

    if (xbtfFile != NULL)
    {
        Xbtf xbtf;

        if ((createDirs) && (outDir == NULL))
        {
            // Just to be sure the user knows what implications this can have
            fprintf(stderr, "Warning: OUTDIR isn't specified so directories");
            fprintf(stderr, "will be created in current dir. If this is");
            fprintf(stderr, "OK, set \"-o ./\"\n");
            exit(1);
        }
        if (print)
        {
            if (!xbtf.open(xbtfFile))
                return 1;

            xbtf.parse();      
            xbtf.printFiles();
        }
        else
        {
            if (!xbtf.open(xbtfFile))
                return 1;

            xbtf.parse();

            if (filename != NULL)
                xbtf.extractFile(filename, outDir, createDirs);
            else
                xbtf.extractAllFiles(outDir, createDirs);
        }
    }
    else
    {
        fprintf(stderr, "Required option (xbtfile) is missing. Use --pack for pack mode.\n");
        printHelp();
        exit(1);
    }

    return 0;
}
