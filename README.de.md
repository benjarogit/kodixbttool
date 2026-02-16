[**Deutsch**](README.de.md) | [English](README.md)

<div align="center">
  <img src="logo.png" alt="kodixbttool" width="200" />
</div>

# kodixbttool

Ein **Kodi-konformes** Kommandozeilen-Tool zum Entpacken und Packen von XBT-Texturarchiven (`.xbt`-Dateien). Skin-Texturen entpacken, bearbeiten und wieder packen – ohne Konfigurationsdatei; alle Pfade übergibst du in der Kommandozeile.

**Zum ersten Mal dabei?** Du lädst eine Datei herunter, öffnest ein Terminal und führst einfache Befehle aus, um die von Kodi-Skins genutzten Textur-Archive zu entpacken oder neu zu packen. In den Tabellen unten steht genau, was du eingeben musst.

---

## Inhaltsverzeichnis

| Abschnitt | Inhalt |
|-----------|--------|
| [Wofür das Tool da ist](#wofür-das-tool-da-ist) | Extrahieren, Auflisten, Packen, ungenutzte Texturen |
| [Downloads](#downloads) | Binary holen und Copy-paste-Befehlstabelle |
| [Wo liegen meine .xbt-Dateien?](#wo-liegen-meine-xbt-dateien) | Typische Kodi-Pfade unter Linux, macOS, Windows |
| [Nutzung](#nutzung) | Ablauf Schritt für Schritt und Befehlsreferenz |
| [Bauen aus dem Quellcode](#bauen-aus-dem-quellcode-optional) | Optional: Tool selbst kompilieren |
| [Referenzen und Lizenz](#referenzen-und-lizenz) | Links und Lizenz |

---

## Wofür das Tool da ist

| Aktion | Beschreibung |
|--------|--------------|
| **Extrahieren** | Eine `.xbt`-Datei in PNG-/JPEG-Bilder und Ordnerstruktur entpacken. |
| **Auflisten** | Dateipfade in einer `.xbt` anzeigen, ohne zu entpacken. |
| **Packen** | Eine `.xbt` aus einem Bildordner erzeugen (LZO, Kodi 17+ / Omega 21). Der Quellordner wird standardmäßig gelöscht; mit `--no-remove-input` bleibt er erhalten. |
| **Ungenutzte** | Texturen anzeigen oder entfernen, die in den Skin-XMLs nicht vorkommen. |

Die Ausgabe entspricht der [Kodi-TexturePacker](https://kodi.wiki/view/TexturePacker)-Spezifikation.

---

## Downloads

Vorkompilierte Binärdateien gibt es auf der [Releases](https://github.com/benjarogit/kodixbttool/releases)-Seite. Kompilieren ist nicht nötig.

| Plattform | Datei |
|-----------|--------|
| Linux x64 | `kodixbttool-linux-x64` |
| macOS | `kodixbttool-macos` |
| Windows x64 | `kodixbttool-windows-x64.exe` |

**Schnellstart:**  
1. Lade die Datei für dein Betriebssystem herunter.  
2. Unter Linux oder macOS: ausführbar machen mit `chmod +x kodixbttool-linux-x64` (bzw. `kodixbttool-macos`).  
3. Terminal (bzw. unter Windows Eingabeaufforderung oder PowerShell) öffnen und die Befehle unten ausführen – Pfade durch deine ersetzen. Liegen Tool oder `.xbt` woanders, den vollständigen Pfad verwenden.

**Copy-paste-Befehle** (Pfade durch deine ersetzen):

| Ich möchte … | Linux / macOS | Windows |
|--------------|----------------|---------|
| **Entpacken** .xbt → Ordner | `./kodixbttool-linux-x64 -o ./Textures_extracted -c ./Textures.xbt` | `.\kodixbttool-windows-x64.exe -o .\Textures_extracted -c .\Textures.xbt` |
| **Packen** Ordner → .xbt | `./kodixbttool-linux-x64 --pack -i ./Textures_extracted -o ./Textures.xbt` | `.\kodixbttool-windows-x64.exe --pack -i .\Textures_extracted -o .\Textures.xbt` |
| **Nur Pfade** anzeigen | `./kodixbttool-linux-x64 -p ./Textures.xbt` | `.\kodixbttool-windows-x64.exe -p .\Textures.xbt` |
| **Ungenutzte** Texturen anzeigen | `./kodixbttool-linux-x64 --list-unused -i ./Textures_extracted` | `.\kodixbttool-windows-x64.exe --list-unused -i .\Textures_extracted` |

---

## Wo liegen meine .xbt-Dateien?

Kodi-Skins speichern die Textur-Archive im **media**-Ordner des Skins. Ersetze `skin.NAME` durch deinen Skin-Namen (z. B. `skin.dokukanal`):

| Plattform | Pfad |
|-----------|------|
| Linux | `~/.kodi/addons/skin.NAME/media/` |
| macOS | `~/Library/Application Support/Kodi/addons/skin.NAME/media/` |
| Windows | `%APPDATA%\Kodi\addons\skin.NAME\media\` (z. B. `C:\Users\DeinName\AppData\Roaming\Kodi\addons\skin.NAME\media\`) |

Terminal (bzw. Eingabeaufforderung oder PowerShell) öffnen, mit `cd` in diesen Ordner wechseln, dann die [Copy-paste-Tabelle](#copy-paste-befehle-pfade-durch-deine-ersetzen) oben mit deinen Dateinamen nutzen.

---

## Nutzung

### Zum ersten Mal? So gehst du vor

**Linux / macOS:**

```bash
cd ~/.kodi/addons/skin.dokukanal/media
./kodixbttool-linux-x64 -o ./Textures_extracted -c ./Textures.xbt
# Bilder in ./Textures_extracted bearbeiten, dann:
./kodixbttool-linux-x64 --list-unused -i ./Textures_extracted
./kodixbttool-linux-x64 --pack -i ./Textures_extracted -o ./Textures.xbt
```

**Windows:**

```powershell
cd $env:APPDATA\Kodi\addons\skin.dokukanal\media
.\kodixbttool-windows-x64.exe -o .\Textures_extracted -c .\Textures.xbt
# Bilder in .\Textures_extracted bearbeiten, dann:
.\kodixbttool-windows-x64.exe --list-unused -i .\Textures_extracted
.\kodixbttool-windows-x64.exe --pack -i .\Textures_extracted -o .\Textures.xbt
```

Liegt das Tool in einem anderen Ordner (z. B. Downloads), in jedem Befehl den vollständigen Pfad zum Tool angeben.

### Referenz: Extrahieren

| Aktion | Linux / macOS | Windows |
|--------|----------------|---------|
| .xbt → Ordner entpacken | `./kodixbttool-linux-x64 -o ./Textures_extracted -c ./Textures.xbt` | `.\kodixbttool-windows-x64.exe -o .\Textures_extracted -c .\Textures.xbt` |
| Nur Pfade anzeigen | `./kodixbttool-linux-x64 -p ./Textures.xbt` | `.\kodixbttool-windows-x64.exe -p .\Textures.xbt` |
| Eine Datei entpacken | `./kodixbttool-linux-x64 -o ./out -c -f pfad/innen/name.png ./Textures.xbt` | `.\kodixbttool-windows-x64.exe -o .\out -c -f pfad\innen\name.png .\Textures.xbt` |

### Referenz: Packen

| Aktion | Linux / macOS | Windows |
|--------|----------------|---------|
| Ordner → .xbt packen | `./kodixbttool-linux-x64 --pack -i ./Textures_extracted -o ./Textures.xbt` | `.\kodixbttool-windows-x64.exe --pack -i .\Textures_extracted -o .\Textures.xbt` |
| Ordner nach dem Packen behalten | `--no-remove-input` anhängen | `--no-remove-input` anhängen |
| Kleinere Datei (Duplikate entfernen) | `--dupecheck` anhängen | `--dupecheck` anhängen |

### Referenz: Ungenutzte Texturen

| Aktion | Linux / macOS | Windows |
|--------|----------------|---------|
| Ungenutzte anzeigen | `./kodixbttool-linux-x64 --list-unused -i ./Textures_extracted` | `.\kodixbttool-windows-x64.exe --list-unused -i .\Textures_extracted` |
| Skin woanders | `--skin-dir=/pfad/zu/skin.NAME` anhängen | `--skin-dir=C:\pfad\zu\skin.NAME` anhängen |
| Beim Packen ungenutzte entfernen | `--remove-unused` anhängen | `--remove-unused` anhängen |

Beim Packen kann gefragt werden: *„Ungenutzte vor dem Packen entfernen? (j/N)“*. Mit `--no-remove-unused` wird nie entfernt (z. B. in Skripten).

---

## Bauen aus dem Quellcode (optional)

Nur nötig, wenn du das Tool selbst kompilieren möchtest.

**Benötigte Bibliotheken:** liblzo2, libpng, libjpeg (z. B. libjpeg-turbo), libgif, libsquish.

| Plattform | Installationsbefehl |
|-----------|---------------------|
| Linux (Debian/Ubuntu) | `sudo apt install liblzo2-dev libpng-dev libjpeg-dev libgif-dev libsquish-dev` |
| Arch Linux | `sudo pacman -S lzo libpng libjpeg-turbo giflib libsquish` |
| macOS | `brew install lzo libpng libjpeg-turbo giflib libsquish` |
| Windows | [vcpkg](https://vcpkg.io/) mit Triplet **x64-windows-static** nutzen (siehe unten) oder [vorkompilierte Binary](https://github.com/benjarogit/kodixbttool/releases) herunterladen. |

**Bauen (Linux / macOS):**

```bash
git clone https://github.com/benjarogit/kodixbttool.git
cd kodixbttool
make
```

Oder mit CMake: `mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && cmake --build .`

**Bauen (Windows mit vcpkg):**

```powershell
vcpkg install libpng libjpeg-turbo lzo giflib libsquish --triplet x64-windows-static
mkdir build; cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE="[vcpkg]/scripts/buildsystems/vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x64-windows-static
cmake --build . --config Release
```

Die ausführbare Datei liegt unter `build\Release\kodixbttool.exe`. Einfacher: [vorkompilierte .exe](https://github.com/benjarogit/kodixbttool/releases) von den Releases nutzen.

**Hinweis:** Mit giflib 5.x ist die GIF-Extraktion nicht unterstützt; PNG und JPEG funktionieren. Tests: `./test.sh` (benötigt .xbt-Dateien in `testdata/` oder Umgebungsvariable `KODIXBT_TEST_XBT_DIR`).

---

## Referenzen und Lizenz

- [Kodi TexturePacker (Wiki)](https://kodi.wiki/view/TexturePacker)
- [KodiTextureTool](https://github.com/kittmaster/KodiTextureTool) – GUI für Windows

**Lizenz:** GPL v3. Dieses Projekt ist ein Fork von [xbtfextractor](https://github.com/larshall/xbtfextractor) von Lars Hall. Modifikationen (C) 2026 Sunny C.
