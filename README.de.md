[**Deutsch**](README.de.md) | [English](README.md)

# kodixbttool

Werkzeug zum **Kodi-konformen** Extrahieren und Packen von Kodi-XBT-Texturarchiven (`.xbt`). Damit kannst du Skin-Texturen (z. B. `Textures.xbt`, `Square.xbt`) auspacken, bearbeiten und wieder in eine einzelne `.xbt`-Datei packen, die Kodi fehlerfrei lädt.

## Wofür es gedacht ist

- **Extrahieren** – Eine `.xbt`-Datei in PNG/JPEG (und optional GIF, wo unterstützt) auspacken und die Verzeichnisstruktur nachbilden.
- **Auflisten** – Alle Dateipfade in einer `.xbt` anzeigen, ohne zu extrahieren.
- **Packen** – Eine `.xbt` aus einem Verzeichnis mit PNG/JPEG-Bildern erzeugen (Kodi 17+ / Omega 21: A8R8G8B8/RGB8, **LZO-komprimiert**). **Standardmäßig** wird das Quellverzeichnis nach erfolgreichem Packen gelöscht, damit es nicht mit Skin oder Add-on mit ausgeliefert wird; mit `--no-remove-input` bleibt es erhalten.
- **Unbenutzte Texturen** – Texturen auflisten oder entfernen, die in den Skin-XMLs nicht referenziert werden (cross-platform). Beim Packen kann optional gefragt werden, ob unbenutzte Texturen vor dem Packen gelöscht werden sollen; oder `--remove-unused` / `--no-remove-unused`.

Das Ausgabeformat entspricht der [Kodi-TexturePacker](https://kodi.wiki/view/TexturePacker)-Spezifikation, damit gepackte Dateien in aktuellen Kodi-Versionen funktionieren.

## Voraussetzungen

Zum Bauen und Ausführen werden folgende Bibliotheken benötigt:

- **liblzo2**
- **libpng**
- **libjpeg** (libjpeg-turbo empfohlen für Alpha)
- **libgif**
- **libsquish**

### Linux

Entwicklungspakete installieren, z. B.:

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

Build wird derzeit nicht vollständig unterstützt. Du kannst [WSL2](https://docs.microsoft.com/de-de/windows/wsl/) nutzen und die Linux-Anleitung befolgen oder die GUI [KodiTextureTool](https://github.com/kittmaster/KodiTextureTool) zum Packen/Entpacken unter Windows verwenden.

### WSL2

Wie unter Linux; Pakete für deine WSL-Distribution (z. B. Ubuntu) wie oben installieren.

## Downloads

Fertige Binaries (Linux, macOS, Windows) und Quellcode-Archive findest du in den [Releases](https://github.com/benjarogit/kodixbttool/releases). **Ein** Binary wird zum **Entpacken** und **Packen** genutzt; lade die Version für deine Plattform herunter oder baue aus dem Quellcode (siehe unten).

**Wie die Pfade funktionieren:** Das Tool **fragt dich beim Start nicht** nach Pfaden. Es gibt **keine Konfigurationsdatei**. Du übergibst alle Pfade immer in der Kommandozeile: welche .xbt-Datei oder welcher Ordner rein soll, wohin ausgepackt oder gepackt werden soll. Die .xbt-Dateien können beliebig liegen; du kannst volle oder relative Pfade angeben, z. B. `./Textures.xbt` oder `C:\Kodi\addons\skin.mein\media\Textures.xbt`.

**Typischer Kodi-Skin-Medienordner** (dort liegen meist `Textures.xbt` und `Square.xbt`):

| Plattform | Typischer Pfad |
|-----------|-----------------|
| Linux     | `~/.kodi/addons/skin.NAME/media/` |
| macOS     | `~/Library/Application Support/Kodi/addons/skin.NAME/media/` |
| Windows   | `%APPDATA%\Kodi\addons\skin.NAME\media\` (z. B. `C:\Users\Du\AppData\Roaming\Kodi\addons\...`) |

Ersetze `skin.NAME` durch den Addon-Ordner deines Skins (z. B. `skin.dokukanal`). Terminal (bzw. Eingabeaufforderung/PowerShell unter Windows) öffnen und die Befehle mit **deinen** Pfaden ausführen.

## Installation aus dem Quellcode

```bash
git clone https://github.com/benjarogit/kodixbttool.git
cd kodixbttool
make
```

Die Binärdatei heißt `kodixbttool`. Optional in ein Verzeichnis im `PATH` kopieren, z. B.:

```bash
cp kodixbttool ~/.local/bin/
```

Falls CMake vorhanden ist:

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

Anschließend `./kodixbttool --help` ausführen.

**Hinweis:** Unter giflib 5.x wird die GIF-Extraktion nicht unterstützt (GIF-Einträge werden mit einer Meldung übersprungen). Die meisten Skin-Texturen sind PNG/JPEG.

**Tests ausführen:** Das Test-Skript (z. B. `./test.sh`) erwartet mindestens eine .xbt-Datei. Setze `KODIXBT_TEST_XBT_DIR` auf ein Verzeichnis mit .xbt-Dateien (z. B. deinen Skin-Medienordner), oder lege .xbt-Dateien in ein Verzeichnis `testdata/` neben dem Skript; wenn nicht gesetzt, nutzt das Skript ggf. einen Standardpfad (z. B. Kodi-Skin-Medienordner).

## Nutzung

**Kurz starten (Beispiel Linux, Skin unter `~/.kodi/addons/skin.dokukanal/media/`):**

```bash
# 1) In den Skin-Medienordner wechseln (oder in den Befehlen unten volle Pfade nutzen)
cd ~/.kodi/addons/skin.dokukanal/media

# 2) Beide Archive auspacken (erzeugt Textures_extracted und Square_extracted)
./kodixbttool -o ./Textures_extracted -c ./Textures.xbt
./kodixbttool -o ./Square_extracted -c ./Square.xbt

# 3) Optional: anzeigen, welche Texturen im Skin nicht genutzt werden
./kodixbttool --list-unused -i ./Textures_extracted

# 4) Wieder packen (z. B. nach Bearbeitung). Es wird gefragt, ob Unbenutzte entfernt werden sollen.
./kodixbttool --pack -i ./Textures_extracted -o ./Textures.xbt
./kodixbttool --pack -i ./Square_extracted -o ./Square.xbt
```

Wenn du die fertige Binärdatei heruntergeladen hast: Pfad zum Tool angeben (z. B. `~/Downloads/kodixbttool`) oder ins PATH legen und `kodixbttool` von überall aufrufen.

### Extrahieren

Alles auspacken und Verzeichnisbaum anlegen (mit **deinen** Pfaden):

```bash
kodixbttool -o ./media/Textures_extracted -c media/Textures.xbt
```

Nur Pfade anzeigen:

```bash
kodixbttool -p media/Textures.xbt
```

Einzelne Datei extrahieren:

```bash
kodixbttool -o ./out -c -f pfad/innen/file.png media/Textures.xbt
```

### Packen

Verzeichnis mit Bildern in eine `.xbt` packen:

```bash
kodixbttool --pack -i ./media/Textures_extracted -o ./media/Textures.xbt
```

Mit `--dupecheck` werden identische Bilddaten nur einmal gespeichert (kleinere Datei):

```bash
kodixbttool --pack -i ./media/Textures_extracted -o ./media/Textures.xbt --dupecheck
```

Nach erfolgreichem Packen wird das Eingabeverzeichnis **standardmäßig gelöscht**, damit es nicht mit Skin/Add-on mitgeliefert wird. Mit `--no-remove-input` bleibt es erhalten.

**Typischer Ablauf:** Extrahieren → Bilder bearbeiten → packen (Eingabeordner wird automatisch gelöscht):

```bash
kodixbttool -o ./media/Textures_extracted -c media/Textures.xbt
# ... Dateien in media/Textures_extracted bearbeiten ...
kodixbttool --pack -i ./media/Textures_extracted -o ./media/Textures.xbt
```

### Unbenutzte Texturen (cross-platform)

Texturen im extrahierten Ordner anzeigen, die **nicht** in den Skin-XMLs vorkommen. Skin-Pfad: `KODI_ADDONS` oder `$HOME/.kodi/addons/skin.dokukanal` (Windows: `%USERPROFILE%\.kodi\addons\skin.dokukanal`), oder mit `--skin-dir` setzen:

```bash
kodixbttool --list-unused -i ./media/Textures_extracted
kodixbttool --list-unused -i ./media/Textures_extracted --skin-dir=/pfad/zu/skin.dokukanal
```

Beim **Packen** kann das Tool unbenutzte Texturen vor dem Packen entfernen, wenn ein Skin-Verzeichnis gefunden wird:

- **Interaktiv:** Bei TTY erscheint: *„N unbenutzte Textur(en). Vor dem Packen entfernen? (j/N)“* – mit `j` werden sie gelöscht, dann gepackt.
- **Ohne Abfrage:** `--remove-unused` entfernt unbenutzte ohne Nachfrage, `--no-remove-unused` entfernt nie (z. B. in Skripten/CI).

Pfade die mit `default` beginnen (Kodi-Fallback-Icons) werden nie als unbenutzt gelistet und nicht gelöscht.

```bash
kodixbttool --pack -i ./media/Textures_extracted -o ./media/Textures.xbt --remove-unused
kodixbttool --pack -i ./media/Textures_extracted -o ./media/Textures.xbt --no-remove-unused
```

## Referenzen

- [Kodi TexturePacker (Wiki)](https://kodi.wiki/view/TexturePacker)
- [KodiTextureTool](https://github.com/kittmaster/KodiTextureTool) – GUI für Windows

## Lizenz

GPL v3. Dieses Projekt ist ein Fork von [xbtfextractor](https://github.com/larshall/xbtfextractor) von Lars Hall. Modifikationen (C) 2026 Sunny C.
