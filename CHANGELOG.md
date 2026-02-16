# Changelog – kodixbttool

All notable changes in this variant compared to the original **xbtfextractor** (Lars Hall) are documented here. The project is maintained at [benjarogit/kodixbttool](https://github.com/benjarogit/kodixbttool).

---

## [Unreleased]

(No planned items.)

---

## [1.0.1] - 2026-02-16

- **macOS build:** CMake now uses `find_package(GIF)` and `find_library` / `find_path` for LZO and libsquish on Apple; workflow sets `CMAKE_PREFIX_PATH` for Homebrew (`/opt/homebrew`, `/usr/local`) so dependencies are found reliably.
- **Release workflow:** Only existing build artifacts are attached to the release (dynamic file list); release-assets job has explicit `contents: write` permission; requires at least the Linux binary to succeed.

---

## Changes vs xbtfextractor

### Build & release

- **Windows build:** getopt is provided by a small port (`getopt_win.c`/`getopt_win.h`) on Windows; directory scan and recursive delete in pack mode use the Windows API (FindFirstFile/FindNextFile, DeleteFile, RemoveDirectory). Build on Windows via CMake with [vcpkg](https://vcpkg.io/) for dependencies (libpng, libjpeg-turbo, lzo, giflib, libsquish).
- **Pre-built binaries:** GitHub Actions workflow (`.github/workflows/build-release.yml`) builds on Linux, macOS, and Windows when a release is published and attaches `kodixbttool-linux-x64`, `kodixbttool-macos`, and `kodixbttool-windows-x64.exe` to the release so users can download a ready-to-run binary per platform.

### Unused texture detection (cross-platform)

- **--list-unused -i INPUT_DIR [--skin-dir=DIR]:** List texture paths in INPUT_DIR that are not referenced in the skin's XML files. Skin root defaults to `KODI_ADDONS` or `$HOME/.kodi/addons/skin.dokukanal` (Windows: `%USERPROFILE%\.kodi\addons\skin.dokukanal`). Paths starting with `default` are excluded (Kodi fallback icons).
- **Pack with optional unused removal:** When packing, if a skin directory is available (default or `--skin-dir`), the tool can remove unused textures before packing:
  - **Interactive:** If stdin is a TTY, prompts "N unused texture(s). Remove before pack? (y/N)".
  - **--remove-unused:** Remove unused without prompting.
  - **--no-remove-unused:** Do not remove (e.g. for scripts/CI).
- Implemented in C++ (`skin_unused.cpp`) with cross-platform directory walk (Unix: opendir/readdir; Windows: FindFirstFile/FindNextFile), XML scan via regex, and TTY detection for the prompt.

### Cleanup after pack

- After a successful pack, the tool removes the **input directory** (default) and, if present, the **.bak file** next to the output (e.g. `Textures.xbt.bak` when packing to `Textures.xbt`). No separate cleanup step needed.

### Documentation (beginner-friendly)

- **No config file, no startup prompt:** All paths are passed on the command line. README explains this clearly and lists typical Kodi skin media paths per platform (Linux, macOS, Windows).
- **Quick start:** Step-by-step example (extract → optional list-unused → pack) with concrete paths so users can copy and adapt.
- Both English (README.md) and German (README.de.md) updated.

---

### General

- **Binary name:** Program is named `kodixbttool` (instead of `xbtfextractor`).
- **Copyright:** Variant with its own copyright; base remains xbtfextractor (C) Lars Hall, GPL v3.

### Extraction (unchanged usage)

- **giflib 5.x:** GIF extraction not supported under giflib 5.x (stub with message); PNG/JPEG extract normally.
- **Buffer/strncat:** Safe buffer handling in `extractMediaFile` (no overflow, correct directory creation).
- **Redundancy:** `parsePath()` called once per file; shared helper `getStrideAndAlpha()` for PNG/JPEG.

### Pack (new)

- **Pack mode:** Pack a directory of PNG/JPEG (and optionally GIF) into a Kodi-compliant .xbt (`--pack -i DIR -o FILE.xbt`).
- **Kodi 17+ / Omega 21 compliant:** A8R8G8B8/RGB8; pack output is **LZO-compressed** (or stored uncompressed if not smaller). Packed .xbt files are readable by Kodi 17+ and Omega 21.
- **Dupe check:** Option `--dupecheck` – identical pixel data stored only once.
- **Cleanup after pack:** After a successful pack, the input directory is **removed by default** so it is not shipped with the skin/add-on. Disable with `--no-remove-input`.

### Build & platform

- **CMake:** `CMakeLists.txt` for unified build (Linux, macOS, optionally Windows).
- **Makefile:** Still supported; produces `kodixbttool`.
- **Platform:** `platform.h` with `xbt_fseek64()` for 64-bit seek (Linux/macOS; Windows prepared).

### Documentation

- **README.md (English)** and **README.de.md (German)** with prerequisites, installation, and usage.
- **Downloads:** Note on releases with pre-built binaries and source archive.

---

*Last updated: 2026.*
