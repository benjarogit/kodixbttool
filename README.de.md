[**Deutsch**](README.de.md) | [English](README.md)

<div align="center">
  <img src="logo.png" alt="kodixbttool" width="200" />
</div>

# kodixbttool

Werkzeug zum **Kodi-konformen** Extrahieren und Packen von Kodi-XBT-Texturarchiven (`.xbt`). Damit kannst du Skin-Texturen (z. B. `Textures.xbt`, `Square.xbt`) auspacken, bearbeiten und wieder in eine einzelne `.xbt`-Datei packen, die Kodi fehlerfrei lädt.

## Wofür es gedacht ist

- **Extrahieren** – Eine `.xbt`-Datei in PNG/JPEG (und optional GIF, wo unterstützt) auspacken und die Verzeichnisstruktur nachbilden.
- **Auflisten** – Alle Dateipfade in einer `.xbt` anzeigen, ohne zu extrahieren.
- **Packen** – Eine `.xbt` aus einem Verzeichnis mit PNG/JPEG-Bildern erzeugen (Kodi 17+ / Omega 21: A8R8G8B8/RGB8, **LZO-komprimiert**). **Standardmäßig** wird das Quellverzeichnis nach erfolgreichem Packen gelöscht, damit es nicht mit Skin oder Add-on mit ausgeliefert wird; mit `--no-remove-input` bleibt es erhalten.
- **Unbenutzte Texturen** – Texturen auflisten oder entfernen, die in den Skin-XMLs nicht referenziert werden (cross-platform). Beim Packen kann optional gefragt werden, ob unbenutzte Texturen vor dem Packen gelöscht werden sollen; oder `--remove-unused` / `--no-remove-unused`.

Das Ausgabeformat entspricht der [Kodi-TexturePacker](https://kodi.wiki/view/TexturePacker)-Spezifikation, damit gepackte Dateien in aktuellen Kodi-Versionen funktionieren.

## Downloads (empfohlen)

Fertige Binaries für **Linux**, **macOS** und **Windows** gibt es in den [Releases](https://github.com/benjarogit/kodixbttool/releases). **Die meisten Nutzer können sie ohne Kompilieren verwenden.**

| Plattform   | Datei |
|-------------|--------|
| Linux x64   | `kodixbttool-linux-x64` |
| macOS       | `kodixbttool-macos` |
| Windows x64 | `kodixbttool-windows-x64.exe` |

1. [Neuestes Release](https://github.com/benjarogit/kodixbttool/releases) öffnen, Datei für dein Betriebssystem herunterladen.
2. Linux/macOS: ausführbar machen (`chmod +x kodixbttool-linux-x64` bzw. `chmod +x kodixbttool-macos`), dann im Terminal mit deinen Pfaden starten (siehe [Nutzung](#nutzung) unten).
3. Windows: `kodixbttool-windows-x64.exe` in Eingabeaufforderung oder PowerShell mit den gewünschten Pfaden ausführen.

**Es gibt keine Konfigurationsdatei.** Du gibst die Pfade immer in derselben Zeile wie den Befehl ein. Unten stehen Copy-paste-Beispiele – ersetze die Pfade durch **deine** Pfade.

**Beispiel – eine .xbt in einen Ordner entpacken (Linux/macOS):**
```bash
./kodixbttool-linux-x64 -o ./Textures_extracted -c ./Textures.xbt
```
Bedeutung: Nimm `Textures.xbt` im aktuellen Ordner und entpacke sie in den neuen Ordner `Textures_extracted`.

**Beispiel – dasselbe unter Windows (Eingabeaufforderung oder PowerShell):**
```powershell
.\kodixbttool-windows-x64.exe -o .\Textures_extracted -c .\Textures.xbt
```

**Beispiel – einen Ordner wieder in eine .xbt packen (Linux/macOS):**
```bash
./kodixbttool-linux-x64 --pack -i ./Textures_extracted -o ./Textures.xbt
```
Bedeutung: Nimm den Ordner `Textures_extracted` und erzeuge daraus `Textures.xbt`. Danach wird der Ordner gelöscht, außer du fügst `--no-remove-input` hinzu.

**Beispiel – dasselbe unter Windows:**
```powershell
.\kodixbttool-windows-x64.exe --pack -i .\Textures_extracted -o .\Textures.xbt
```

**Beispiel – nur anzeigen, was in einer .xbt steckt (ohne Entpacken):**
```bash
./kodixbttool-linux-x64 -p ./Textures.xbt
```
Windows: `.\kodixbttool-windows-x64.exe -p .\Textures.xbt`

**Beispiel – anzeigen, welche Texturen im Skin nicht genutzt werden (kannst du vor dem Packen entfernen):**
```bash
./kodixbttool-linux-x64 --list-unused -i ./Textures_extracted
```
Windows: `.\kodixbttool-windows-x64.exe --list-unused -i .\Textures_extracted`

Wenn deine .xbt oder dein Ordner woanders liegt, nutze den vollen Pfad, z. B.  
Linux: `-c /home/du/.kodi/addons/skin.dokukanal/media/Textures.xbt`  
Windows: `-c C:\Users\Du\AppData\Roaming\Kodi\addons\skin.dokukanal\media\Textures.xbt`

## Voraussetzungen (nur zum Bauen aus dem Quellcode)

Zum Bauen und Ausführen werden folgende Bibliotheken benötigt:

- **liblzo2**
- **libpng**
- **libjpeg** (libjpeg-turbo empfohlen für Alpha)
- **libgif**
- **libsquish**

### Linux

Entwicklungspakete installieren, z. B.:

- **Debian / Ubuntu:**  
  `sudo apt install liblzo2-dev libpng-dev libjpeg-dev libgif-dev libsquish-dev`
- **Arch:**  
  `sudo pacman -S lzo libpng libjpeg-turbo giflib libsquish`
- **Fedora:**  
  `sudo dnf install lzo-devel libpng-devel libjpeg-turbo-devel giflib-devel libsquish-devel`

### macOS

Mit [Homebrew](https://brew.sh):

```bash
brew install lzo libpng libjpeg-turbo giflib libsquish
```

### Windows

[vcpkg](https://vcpkg.io/) installieren, dann Abhängigkeiten mit **static**-Triplet installieren und mit CMake bauen:

```powershell
vcpkg install libpng libjpeg-turbo lzo giflib libsquish --triplet x64-windows-static
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE="C:/pfad/zu/vcpkg/scripts/buildsystems/vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x64-windows-static
cmake --build . --config Release
```

Das Binary liegt unter `build\Release\kodixbttool.exe`. **Einfacher:** [fertiges Binary](https://github.com/benjarogit/kodixbttool/releases) (`kodixbttool-windows-x64.exe`) aus den Releases nutzen, dann brauchst du vcpkg und Build nicht. Oder [WSL2](https://docs.microsoft.com/de-de/windows/wsl/) und die Linux-Anleitung verwenden.

### WSL2

Wie unter Linux; Pakete für deine WSL-Distribution (z. B. Ubuntu) wie oben installieren.

## Wo liegen meine .xbt-Dateien?

Kodi-Skins speichern die Textur-Archive im **media**-Ordner des Skins. Typische Orte (ersetze `skin.NAME` durch deinen Skin, z. B. `skin.dokukanal`):

| Plattform | Pfad zum Ordner mit Textures.xbt / Square.xbt |
|-----------|------------------------------------------------|
| Linux     | `~/.kodi/addons/skin.NAME/media/` |
| macOS     | `~/Library/Application Support/Kodi/addons/skin.NAME/media/` |
| Windows   | `C:\Users\DEINNAME\AppData\Roaming\Kodi\addons\skin.NAME\media\` |

Terminal (Linux/macOS) oder Eingabeaufforderung/PowerShell (Windows) öffnen, in diesen Ordner wechseln (`cd` mit dem Pfad oben), dann die [Copy-paste-Beispiele](#downloads-empfohlen) aus dem Download-Abschnitt mit deinen Dateinamen nutzen.

## Bauen aus dem Quellcode (optional)

Wenn du kodixbttool selbst bauen willst (z. B. für die neueste Git-Version), installiere die [Bibliotheken unter Voraussetzungen](#voraussetzungen-nur-zum-bauen-aus-dem-quellcode) für deine Plattform, dann:

```bash
git clone https://github.com/benjarogit/kodixbttool.git
cd kodixbttool
make
```

Die Binärdatei heißt `kodixbttool`. Optional in ein Verzeichnis im `PATH` kopieren, z. B.:

```bash
cp kodixbttool ~/.local/bin/
```

Falls CMake vorhanden ist:

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

Anschließend `./kodixbttool --help` ausführen. Unter Windows mit vcpkg das **x64-windows-static**-Triplet verwenden (siehe [Windows](#windows)), damit die Laufzeitbibliothek passt – oder das fertige Binary aus den Releases nutzen.

**Hinweis:** Unter giflib 5.x wird die GIF-Extraktion nicht unterstützt (GIF-Einträge werden mit einer Meldung übersprungen). Die meisten Skin-Texturen sind PNG/JPEG.

**Tests ausführen:** Das Test-Skript (z. B. `./test.sh`) erwartet mindestens eine .xbt-Datei. Setze `KODIXBT_TEST_XBT_DIR` auf ein Verzeichnis mit .xbt-Dateien (z. B. deinen Skin-Medienordner), oder lege .xbt-Dateien in ein Verzeichnis `testdata/` neben dem Skript; wenn nicht gesetzt, nutzt das Skript ggf. einen Standardpfad (z. B. Kodi-Skin-Medienordner).

## Nutzung

### Das erste Mal? Kompletter Ablauf (copy-paste und Pfade anpassen)

**Linux / macOS** (z. B. Skin unter `~/.kodi/addons/skin.dokukanal/media/`):

```bash
cd ~/.kodi/addons/skin.dokukanal/media
./kodixbttool-linux-x64 -o ./Textures_extracted -c ./Textures.xbt
# Jetzt die Bilder in ./Textures_extracted mit einem Bildprogramm bearbeiten.
./kodixbttool-linux-x64 --list-unused -i ./Textures_extracted
# Optional: das Tool fragt „Unbenutzte vor dem Packen entfernen? (j/N)“ – j oder n antworten.
./kodixbttool-linux-x64 --pack -i ./Textures_extracted -o ./Textures.xbt
```

**Windows** (z. B. Skin unter `C:\Users\Du\AppData\Roaming\Kodi\addons\skin.dokukanal\media\`):

```powershell
cd $env:APPDATA\Kodi\addons\skin.dokukanal\media
.\kodixbttool-windows-x64.exe -o .\Textures_extracted -c .\Textures.xbt
# Jetzt die Bilder in .\Textures_extracted mit einem Bildprogramm bearbeiten.
.\kodixbttool-windows-x64.exe --list-unused -i .\Textures_extracted
.\kodixbttool-windows-x64.exe --pack -i .\Textures_extracted -o .\Textures.xbt
```

Wenn das Tool woanders liegt (z. B. Downloads), den vollen Pfad angeben, z. B.  
`~/Downloads/kodixbttool-linux-x64` oder `C:\Users\Du\Downloads\kodixbttool-windows-x64.exe`.

### Extrahieren

**Eine .xbt in einen neuen Ordner entpacken** (alle Bilder und Ordnerstruktur):

| Linux/macOS | Windows |
|-------------|---------|
| `./kodixbttool-linux-x64 -o ./Textures_extracted -c ./Textures.xbt` | `.\kodixbttool-windows-x64.exe -o .\Textures_extracted -c .\Textures.xbt` |

**Nur Pfade anzeigen**, die in der .xbt stecken (ohne Entpacken):

| Linux/macOS | Windows |
|-------------|---------|
| `./kodixbttool-linux-x64 -p ./Textures.xbt` | `.\kodixbttool-windows-x64.exe -p .\Textures.xbt` |

**Eine einzelne Datei** aus der .xbt extrahieren (z. B. ein Bild):

| Linux/macOS | Windows |
|-------------|---------|
| `./kodixbttool-linux-x64 -o ./out -c -f pfad/innen/name.png ./Textures.xbt` | `.\kodixbttool-windows-x64.exe -o .\out -c -f pfad\innen\name.png .\Textures.xbt` |

Den Pfad so verwenden, wie er bei `-p` angezeigt wird.

### Packen

**Einen Bild-Ordner in eine .xbt packen:**

| Linux/macOS | Windows |
|-------------|---------|
| `./kodixbttool-linux-x64 --pack -i ./Textures_extracted -o ./Textures.xbt` | `.\kodixbttool-windows-x64.exe --pack -i .\Textures_extracted -o .\Textures.xbt` |

Nach erfolgreichem Packen wird der Ordner (`Textures_extracted`) **standardmäßig gelöscht**. Zum Behalten `--no-remove-input` anhängen:

| Linux/macOS | Windows |
|-------------|---------|
| `./kodixbttool-linux-x64 --pack -i ./Textures_extracted -o ./Textures.xbt --no-remove-input` | `.\kodixbttool-windows-x64.exe --pack -i .\Textures_extracted -o .\Textures.xbt --no-remove-input` |

**Kleinere Datei** (gleiche Bilder nur einmal): `--dupecheck` anhängen:

| Linux/macOS | Windows |
|-------------|---------|
| `./kodixbttool-linux-x64 --pack -i ./Textures_extracted -o ./Textures.xbt --dupecheck` | `.\kodixbttool-windows-x64.exe --pack -i .\Textures_extracted -o .\Textures.xbt --dupecheck` |

### Unbenutzte Texturen

**Anzeigen**, welche Texturen im extrahierten Ordner im Skin **nicht** genutzt werden (zum Verkleinern vor dem Packen):

| Linux/macOS | Windows |
|-------------|---------|
| `./kodixbttool-linux-x64 --list-unused -i ./Textures_extracted` | `.\kodixbttool-windows-x64.exe --list-unused -i .\Textures_extracted` |

Das Tool sucht den Skin am Standardort (z. B. `~/.kodi/addons/skin.dokukanal`). Liegt dein Skin woanders, explizit angeben:

| Linux/macOS | Windows |
|-------------|---------|
| `./kodixbttool-linux-x64 --list-unused -i ./Textures_extracted --skin-dir=/pfad/zu/skin.dokukanal` | `.\kodixbttool-windows-x64.exe --list-unused -i .\Textures_extracted --skin-dir=C:\pfad\zu\skin.dokukanal` |

**Beim Packen** kann das Tool unbenutzte Texturen vorher entfernen.  
- Es fragt ggf.: *„N unbenutzte Textur(en). Vor dem Packen entfernen? (j/N)“* – mit `j` oder `n` antworten.  
- Oder erzwingen: `--remove-unused` (immer entfernen) oder `--no-remove-unused` (nie entfernen).

| Linux/macOS | Windows |
|-------------|---------|
| `./kodixbttool-linux-x64 --pack -i ./Textures_extracted -o ./Textures.xbt --remove-unused` | `.\kodixbttool-windows-x64.exe --pack -i .\Textures_extracted -o .\Textures.xbt --remove-unused` |

## Referenzen

- [Kodi TexturePacker (Wiki)](https://kodi.wiki/view/TexturePacker)
- [KodiTextureTool](https://github.com/kittmaster/KodiTextureTool) – GUI für Windows

## Lizenz

GPL v3. Dieses Projekt ist ein Fork von [xbtfextractor](https://github.com/larshall/xbtfextractor) von Lars Hall. Modifikationen (C) 2026 Sunny C.
