[Deutsch](README.de.md) | **English**

<div align="center">
  <img src="logo.png" alt="kodixbttool" width="200" />
</div>

# kodixbttool

A **Kodi-compliant** command-line tool to extract and pack XBT texture archives (`.xbt` files). Extract skin textures, edit them, then pack them back—no configuration file needed; you pass all paths on the command line.

**New to this?** You download one file, open a terminal, and run simple commands to unpack or repack texture archives used by Kodi skins. The tables below show exactly what to type.

---

## Table of contents

| Section | Description |
|--------|-------------|
| [What it does](#what-it-does) | Extract, list, pack, find unused textures |
| [Downloads](#downloads) | Get the binary and copy-paste command table |
| [Where are my .xbt files?](#where-are-my-xbt-files) | Typical Kodi paths on Linux, macOS, Windows |
| [Usage](#usage) | Step-by-step workflow and command reference |
| [Building from source](#building-from-source-optional) | Optional: compile the tool yourself |
| [References and license](#references-and-license) | Links and license |

---

## What it does

| Action | Description |
|--------|-------------|
| **Extract** | Unpack a `.xbt` file into PNG/JPEG images and a folder structure. |
| **List** | Show file paths inside a `.xbt` without extracting. |
| **Pack** | Build a `.xbt` from a folder of images (LZO compression, Kodi 17+ / Omega 21). The input folder is removed by default; use `--no-remove-input` to keep it. |
| **Unused** | List or remove textures that are not referenced in the skin’s XML files. |

Output matches the [Kodi TexturePacker](https://kodi.wiki/view/TexturePacker) specification.

---

## Downloads

Pre-built binaries are available on the [Releases](https://github.com/benjarogit/kodixbttool/releases) page. No compilation needed.

| Platform | File |
|----------|------|
| Linux x64 | `kodixbttool-linux-x64` |
| macOS | `kodixbttool-macos` |
| Windows x64 | `kodixbttool-windows-x64.exe` |

**Quick start:**  
1. Download the file for your operating system.  
2. On Linux or macOS: make it executable with `chmod +x kodixbttool-linux-x64` (or `kodixbttool-macos`).  
3. Open a terminal (or Command Prompt / PowerShell on Windows) and run the commands below, replacing the paths with your own. Use full paths if the tool or the `.xbt` file is in another folder.

**Copy-paste commands** (replace the paths with yours):

| I want to… | Linux / macOS | Windows |
|------------|----------------|---------|
| **Extract** .xbt → folder | `./kodixbttool-linux-x64 -o ./Textures_extracted -c ./Textures.xbt` | `.\kodixbttool-windows-x64.exe -o .\Textures_extracted -c .\Textures.xbt` |
| **Pack** folder → .xbt | `./kodixbttool-linux-x64 --pack -i ./Textures_extracted -o ./Textures.xbt` | `.\kodixbttool-windows-x64.exe --pack -i .\Textures_extracted -o .\Textures.xbt` |
| **List** paths only | `./kodixbttool-linux-x64 -p ./Textures.xbt` | `.\kodixbttool-windows-x64.exe -p .\Textures.xbt` |
| **List unused** textures | `./kodixbttool-linux-x64 --list-unused -i ./Textures_extracted` | `.\kodixbttool-windows-x64.exe --list-unused -i .\Textures_extracted` |

---

## Where are my .xbt files?

Kodi skins store texture archives in the skin’s **media** folder. Replace `skin.NAME` with your skin name (e.g. `skin.dokukanal`):

| Platform | Path |
|----------|------|
| Linux | `~/.kodi/addons/skin.NAME/media/` |
| macOS | `~/Library/Application Support/Kodi/addons/skin.NAME/media/` |
| Windows | `%APPDATA%\Kodi\addons\skin.NAME\media\` (e.g. `C:\Users\YourName\AppData\Roaming\Kodi\addons\skin.NAME\media\`) |

Open a terminal (or Command Prompt / PowerShell), go to that folder with `cd`, then use the [copy-paste table](#copy-paste-commands-replace-the-paths-with-yours) above with your file names.

---

## Usage

### First time? Follow this workflow

**Linux / macOS:**

```bash
cd ~/.kodi/addons/skin.dokukanal/media
./kodixbttool-linux-x64 -o ./Textures_extracted -c ./Textures.xbt
# Edit images in ./Textures_extracted, then:
./kodixbttool-linux-x64 --list-unused -i ./Textures_extracted
./kodixbttool-linux-x64 --pack -i ./Textures_extracted -o ./Textures.xbt
```

**Windows:**

```powershell
cd $env:APPDATA\Kodi\addons\skin.dokukanal\media
.\kodixbttool-windows-x64.exe -o .\Textures_extracted -c .\Textures.xbt
# Edit images in .\Textures_extracted, then:
.\kodixbttool-windows-x64.exe --list-unused -i .\Textures_extracted
.\kodixbttool-windows-x64.exe --pack -i .\Textures_extracted -o .\Textures.xbt
```

If the tool is in another folder (e.g. Downloads), use its full path in each command.

### Reference: Extract

| Task | Linux / macOS | Windows |
|------|----------------|---------|
| Extract .xbt → folder | `./kodixbttool-linux-x64 -o ./Textures_extracted -c ./Textures.xbt` | `.\kodixbttool-windows-x64.exe -o .\Textures_extracted -c .\Textures.xbt` |
| List paths only | `./kodixbttool-linux-x64 -p ./Textures.xbt` | `.\kodixbttool-windows-x64.exe -p .\Textures.xbt` |
| Extract one file | `./kodixbttool-linux-x64 -o ./out -c -f path/inside/name.png ./Textures.xbt` | `.\kodixbttool-windows-x64.exe -o .\out -c -f path\inside\name.png .\Textures.xbt` |

### Reference: Pack

| Task | Linux / macOS | Windows |
|------|----------------|---------|
| Pack folder → .xbt | `./kodixbttool-linux-x64 --pack -i ./Textures_extracted -o ./Textures.xbt` | `.\kodixbttool-windows-x64.exe --pack -i .\Textures_extracted -o .\Textures.xbt` |
| Keep folder after packing | Add `--no-remove-input` | Add `--no-remove-input` |
| Smaller file (deduplicate) | Add `--dupecheck` | Add `--dupecheck` |

### Reference: Unused textures

| Task | Linux / macOS | Windows |
|------|----------------|---------|
| List unused | `./kodixbttool-linux-x64 --list-unused -i ./Textures_extracted` | `.\kodixbttool-windows-x64.exe --list-unused -i .\Textures_extracted` |
| Skin in another location | Add `--skin-dir=/path/to/skin.NAME` | Add `--skin-dir=C:\path\to\skin.NAME` |
| Remove unused when packing | Add `--remove-unused` | Add `--remove-unused` |

When packing, the tool may ask *"Remove unused before pack? (y/N)"*. Use `--no-remove-unused` to never remove (e.g. in scripts).

---

## Building from source (optional)

Only needed if you want to compile the tool yourself.

**Required libraries:** liblzo2, libpng, libjpeg (e.g. libjpeg-turbo), libgif, libsquish.

| Platform | Install command |
|----------|-----------------|
| Linux (Debian/Ubuntu) | `sudo apt install liblzo2-dev libpng-dev libjpeg-dev libgif-dev libsquish-dev` |
| Arch Linux | `sudo pacman -S lzo libpng libjpeg-turbo giflib libsquish` |
| macOS | `brew install lzo libpng libjpeg-turbo giflib libsquish` |
| Windows | Use [vcpkg](https://vcpkg.io/) with the `x64-windows-static` triplet (see below), or download the [pre-built binary](https://github.com/benjarogit/kodixbttool/releases). |

**Build (Linux / macOS):**

```bash
git clone https://github.com/benjarogit/kodixbttool.git
cd kodixbttool
make
```

Or with CMake: `mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && cmake --build .`

**Build (Windows with vcpkg):**

```powershell
vcpkg install libpng libjpeg-turbo lzo giflib libsquish --triplet x64-windows-static
mkdir build; cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE="[vcpkg]/scripts/buildsystems/vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x64-windows-static
cmake --build . --config Release
```

The executable will be at `build\Release\kodixbttool.exe`. Easier option: use the [pre-built .exe](https://github.com/benjarogit/kodixbttool/releases) from Releases.

**Note:** With giflib 5.x, GIF extraction is not supported; PNG and JPEG work. To run tests: `./test.sh` (requires .xbt files in `testdata/` or set `KODIXBT_TEST_XBT_DIR`).

---

## References and license

- [Kodi TexturePacker (Wiki)](https://kodi.wiki/view/TexturePacker)
- [KodiTextureTool](https://github.com/kittmaster/KodiTextureTool) – GUI for Windows

**License:** GPL v3. This project is a fork of [xbtfextractor](https://github.com/larshall/xbtfextractor) by Lars Hall. Modifications (C) 2026 Sunny C.
