[Deutsch](README.de.md) | **English**

<div align="center">
  <img src="logo.png" alt="kodixbttool" width="200" />
</div>

# kodixbttool

**Kodi-conform** extract and pack tool for Kodi XBT texture archives (`.xbt`). Use it to unpack skin textures (e.g. `Textures.xbt`, `Square.xbt`), edit them, and pack them back into a single `.xbt` file that Kodi loads correctly.

## What it does

- **Extract** – Unpack a `.xbt` file to PNG/JPEG (and optionally GIF where supported) and recreate the directory tree.
- **List** – Print all file paths inside a `.xbt` without extracting.
- **Pack** – Build a `.xbt` from a directory of PNG/JPEG images (Kodi 17+ / Omega 21: A8R8G8B8/RGB8, **LZO-compressed**). **By default** the source directory is removed after a successful pack so it is not shipped with your skin or add-on; use `--no-remove-input` to keep it.
- **Unused textures** – List or remove textures that are not referenced in the skin's XML files (cross-platform). When packing, you can be prompted to remove unused textures before packing, or use `--remove-unused` / `--no-remove-unused`.

Output format matches the [Kodi TexturePacker](https://kodi.wiki/view/TexturePacker) specification so that packed files work in current Kodi versions.

## Downloads (recommended)

Pre-built binaries for **Linux**, **macOS** and **Windows** are in the [Releases](https://github.com/benjarogit/kodixbttool/releases) section. **Most users can use these without compiling.**

| Platform | Download |
|----------|----------|
| Linux x64 | `kodixbttool-linux-x64` |
| macOS     | `kodixbttool-macos` |
| Windows x64 | `kodixbttool-windows-x64.exe` |

1. Open the [latest release](https://github.com/benjarogit/kodixbttool/releases), download the file for your OS.
2. On Linux/macOS: make it executable (`chmod +x kodixbttool-linux-x64` or `chmod +x kodixbttool-macos`), then run it from the terminal with your paths (see [Usage](#usage) below).
3. On Windows: run `kodixbttool-windows-x64.exe` from Command Prompt or PowerShell with your paths.

**No config file.** You always type the paths in the same line as the command. Below are copy-paste examples – replace the paths with **your** paths.

**Example – extract a .xbt into a folder (Linux/macOS):**
```bash
./kodixbttool-linux-x64 -o ./Textures_extracted -c ./Textures.xbt
```
Meaning: take `Textures.xbt` in the current folder, unpack it into the new folder `Textures_extracted`.

**Example – same on Windows (Command Prompt or PowerShell):**
```powershell
.\kodixbttool-windows-x64.exe -o .\Textures_extracted -c .\Textures.xbt
```

**Example – pack a folder back into a .xbt (Linux/macOS):**
```bash
./kodixbttool-linux-x64 --pack -i ./Textures_extracted -o ./Textures.xbt
```
Meaning: take the folder `Textures_extracted`, create `Textures.xbt` from it. After that, the folder is removed unless you add `--no-remove-input`.

**Example – same on Windows:**
```powershell
.\kodixbttool-windows-x64.exe --pack -i .\Textures_extracted -o .\Textures.xbt
```

**Example – only list what’s inside a .xbt (no extraction):**
```bash
./kodixbttool-linux-x64 -p ./Textures.xbt
```
Windows: `.\kodixbttool-windows-x64.exe -p .\Textures.xbt`

**Example – list textures that are not used in the skin (then you can remove them before packing):**
```bash
./kodixbttool-linux-x64 --list-unused -i ./Textures_extracted
```
Windows: `.\kodixbttool-windows-x64.exe --list-unused -i .\Textures_extracted`

If your .xbt or folder is somewhere else, use the full path, e.g.  
Linux: `-c /home/you/.kodi/addons/skin.dokukanal/media/Textures.xbt`  
Windows: `-c C:\Users\You\AppData\Roaming\Kodi\addons\skin.dokukanal\media\Textures.xbt`

## Requirements (only for building from source)

You need these libraries to build and run kodixbttool:

- **liblzo2**
- **libpng**
- **libjpeg** (libjpeg-turbo recommended for alpha)
- **libgif**
- **libsquish**

### Linux

Install development packages, for example:

- **Debian / Ubuntu:**  
  `sudo apt install liblzo2-dev libpng-dev libjpeg-dev libgif-dev libsquish-dev`
- **Arch:**  
  `sudo pacman -S lzo libpng libjpeg-turbo giflib libsquish`
- **Fedora:**  
  `sudo dnf install lzo-devel libpng-devel libjpeg-turbo-devel giflib-devel libsquish-devel`

### macOS

With [Homebrew](https://brew.sh):

```bash
brew install lzo libpng libjpeg-turbo giflib libsquish
```

### Windows

Install [vcpkg](https://vcpkg.io/), then install dependencies (static triplet so the runtime matches) and build with CMake:

```powershell
vcpkg install libpng libjpeg-turbo lzo giflib libsquish --triplet x64-windows-static
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE="[path-to-vcpkg]/scripts/buildsystems/vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x64-windows-static
cmake --build . --config Release
```

The binary is `build\Release\kodixbttool.exe`. **Easier:** use the [pre-built binary](https://github.com/benjarogit/kodixbttool/releases) (`kodixbttool-windows-x64.exe`) from Releases so you don’t need to install vcpkg or compile. Or use [WSL2](https://docs.microsoft.com/en-us/windows/wsl/) and follow the Linux instructions.

### WSL2

Same as Linux; install the packages for your WSL distribution (e.g. Ubuntu) as above.

## Where are my .xbt files?

Kodi skins store texture archives in the skin’s **media** folder. Typical locations (replace `skin.NAME` with your skin, e.g. `skin.dokukanal`):

| Platform | Path to folder with Textures.xbt / Square.xbt |
|----------|-------------------------------------------------|
| Linux    | `~/.kodi/addons/skin.NAME/media/` |
| macOS    | `~/Library/Application Support/Kodi/addons/skin.NAME/media/` |
| Windows  | `C:\Users\YOURNAME\AppData\Roaming\Kodi\addons\skin.NAME\media\` |

Open a terminal (Linux/macOS) or Command Prompt / PowerShell (Windows), go to that folder (`cd` to the path above), then use the [copy-paste examples](#downloads-recommended) from the Downloads section with your file names.

## Building from source (optional)

If you prefer to compile kodixbttool yourself (e.g. to get the latest git version), install the [libraries listed under Requirements](#requirements-only-for-building-from-source) for your platform, then:

```bash
git clone https://github.com/benjarogit/kodixbttool.git
cd kodixbttool
make
```

The binary is `kodixbttool`. Optionally copy it to a directory in your `PATH`, e.g.:

```bash
cp kodixbttool ~/.local/bin/
```

If you have CMake:

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

Then run `./kodixbttool --help`. On Windows with vcpkg, use the **x64-windows-static** triplet so the CRT matches (see [Windows](#windows) above); or use the pre-built binary from Releases.

**Note:** With giflib 5.x, GIF extraction is not supported (GIF entries are skipped with a message). Most skin textures are PNG/JPEG.

**Running tests:** The test script (e.g. `./test.sh`) expects at least one .xbt file. Set `KODIXBT_TEST_XBT_DIR` to a directory containing .xbt files (e.g. your skin media folder), or place .xbt files in a `testdata/` directory next to the script; if unset, the script may use a default path (e.g. a Kodi skin media directory).

## Usage

### First time? Complete workflow (copy-paste and change paths)

**Linux / macOS** (e.g. skin in `~/.kodi/addons/skin.dokukanal/media/`):

```bash
cd ~/.kodi/addons/skin.dokukanal/media
./kodixbttool-linux-x64 -o ./Textures_extracted -c ./Textures.xbt
# Now edit the images in ./Textures_extracted with any image editor.
./kodixbttool-linux-x64 --list-unused -i ./Textures_extracted
# Optional: the tool may ask "Remove unused before pack? (y/N)" – answer y or n.
./kodixbttool-linux-x64 --pack -i ./Textures_extracted -o ./Textures.xbt
```

**Windows** (e.g. skin in `C:\Users\You\AppData\Roaming\Kodi\addons\skin.dokukanal\media\`):

```powershell
cd $env:APPDATA\Kodi\addons\skin.dokukanal\media
.\kodixbttool-windows-x64.exe -o .\Textures_extracted -c .\Textures.xbt
# Now edit the images in .\Textures_extracted with any image editor.
.\kodixbttool-windows-x64.exe --list-unused -i .\Textures_extracted
.\kodixbttool-windows-x64.exe --pack -i .\Textures_extracted -o .\Textures.xbt
```

If the tool is in another folder (e.g. Downloads), use the full path to it, e.g.  
`~/Downloads/kodixbttool-linux-x64` or `C:\Users\You\Downloads\kodixbttool-windows-x64.exe`.

### Extract

**Extract a .xbt into a new folder** (all images and folder structure):

| Linux/macOS | Windows |
|-------------|---------|
| `./kodixbttool-linux-x64 -o ./Textures_extracted -c ./Textures.xbt` | `.\kodixbttool-windows-x64.exe -o .\Textures_extracted -c .\Textures.xbt` |

**Only list paths** inside the .xbt (no extraction):

| Linux/macOS | Windows |
|-------------|---------|
| `./kodixbttool-linux-x64 -p ./Textures.xbt` | `.\kodixbttool-windows-x64.exe -p .\Textures.xbt` |

**Extract a single file** from the .xbt (e.g. one image):

| Linux/macOS | Windows |
|-------------|---------|
| `./kodixbttool-linux-x64 -o ./out -c -f path/inside/name.png ./Textures.xbt` | `.\kodixbttool-windows-x64.exe -o .\out -c -f path\inside\name.png .\Textures.xbt` |

Use the path as it appears in the list from `-p`.

### Pack

**Pack a folder of images into a .xbt:**

| Linux/macOS | Windows |
|-------------|---------|
| `./kodixbttool-linux-x64 --pack -i ./Textures_extracted -o ./Textures.xbt` | `.\kodixbttool-windows-x64.exe --pack -i .\Textures_extracted -o .\Textures.xbt` |

After a successful pack, the folder (`Textures_extracted`) is **deleted by default**. To keep it, add `--no-remove-input`:

| Linux/macOS | Windows |
|-------------|---------|
| `./kodixbttool-linux-x64 --pack -i ./Textures_extracted -o ./Textures.xbt --no-remove-input` | `.\kodixbttool-windows-x64.exe --pack -i .\Textures_extracted -o .\Textures.xbt --no-remove-input` |

**Smaller file** (identical images stored once): add `--dupecheck`:

| Linux/macOS | Windows |
|-------------|---------|
| `./kodixbttool-linux-x64 --pack -i ./Textures_extracted -o ./Textures.xbt --dupecheck` | `.\kodixbttool-windows-x64.exe --pack -i .\Textures_extracted -o .\Textures.xbt --dupecheck` |

### Unused textures

**List textures** in the extracted folder that are **not** used in the skin (so you can remove them to shrink the pack):

| Linux/macOS | Windows |
|-------------|---------|
| `./kodixbttool-linux-x64 --list-unused -i ./Textures_extracted` | `.\kodixbttool-windows-x64.exe --list-unused -i .\Textures_extracted` |

The tool looks for the skin in the default place (e.g. `~/.kodi/addons/skin.dokukanal`). If your skin is elsewhere, set it explicitly:

| Linux/macOS | Windows |
|-------------|---------|
| `./kodixbttool-linux-x64 --list-unused -i ./Textures_extracted --skin-dir=/path/to/skin.dokukanal` | `.\kodixbttool-windows-x64.exe --list-unused -i .\Textures_extracted --skin-dir=C:\path\to\skin.dokukanal` |

**When packing:** the tool can remove unused textures before packing.  
- It may ask: *"N unused texture(s). Remove before pack? (y/N)"* – answer `y` or `n`.  
- Or force it: `--remove-unused` (always remove) or `--no-remove-unused` (never remove).

| Linux/macOS | Windows |
|-------------|---------|
| `./kodixbttool-linux-x64 --pack -i ./Textures_extracted -o ./Textures.xbt --remove-unused` | `.\kodixbttool-windows-x64.exe --pack -i .\Textures_extracted -o .\Textures.xbt --remove-unused` |

## References

- [Kodi TexturePacker (Wiki)](https://kodi.wiki/view/TexturePacker)
- [KodiTextureTool](https://github.com/kittmaster/KodiTextureTool) – GUI for Windows

## License

GPL v3. This project is a fork of [xbtfextractor](https://github.com/larshall/xbtfextractor) by Lars Hall. Modifications (C) 2026 Sunny C.
