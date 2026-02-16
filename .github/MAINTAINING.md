# Maintainer notes (kodixbttool)

Kurze Notizen für dich als Maintainer – nicht für Endnutzer gedacht.

## Release-Workflow (CI)

Wenn du auf GitHub ein **Release veröffentlichst** (oder unter *Actions → Build release → Run workflow* manuell startest), läuft der Workflow **Build release**:

1. **Build-Job:** Pro Plattform (Linux, macOS, Windows) werden Repo ausgecheckt, Abhängigkeiten installiert (apt / Homebrew / vcpkg), CMake ausgeführt und das Binary gebaut.
2. **Artifacts:** Jedes Binary wird als Artifact hochgeladen (`kodixbttool-linux-x64`, `kodixbttool-macos`, `kodixbttool-windows-x64.exe`).
3. **Release-Assets:** Ein zweiter Job lädt alle Artifacts herunter, benennt sie um und hängt sie mit **softprops/action-gh-release** an **dasselbe Release** (an den Release-Tag) an. Auf der Release-Seite stehen dann Quellcode-Zip und diese Binaries zum Download.

Fertige Binaries entstehen also automatisch beim Veröffentlichen eines Releases; manueller Build oder Upload ist nicht nötig.

## Neues Release erstellen (immer mit passendem Changelog)

1. **CHANGELOG.md** anpassen (vor dem Release):
   - Neuen Versionsblock anlegen, z. B. `## [1.0.2] - YYYY-MM-DD`.
   - Darunter die Änderungen für diese Version (Stichpunkte).
   - `[Unreleased]` leer lassen oder „(No planned items.)“.

2. **Committen und pushen** (inkl. CHANGELOG + Code-Änderungen):
   ```bash
   cd ~/Dokumente/kodixbttool
   git add -A && git status
   git commit -m "Release v1.0.2: ..."   # Kurzbeschreibung
   git push origin master
   ```

3. **Release auf GitHub anlegen** (Tag = Version, Beschreibung = Changelog):
   - **Releases → Draft a new release**
   - **Choose a tag:** z. B. `v1.0.2` eingeben → **Create new tag: v1.0.2**
   - **Release title:** z. B. `v1.0.2`
   - **Describe this release:** Den **Changelog-Abschnitt** für diese Version aus CHANGELOG.md reinkopieren (nur die Punkte unter `## [1.0.2]`, ohne die Überschrift), z. B.:
     ```text
     - **macOS build:** Fix 64-bit seek on Apple: use `fseeko` on macOS and `fseeko64` on Linux in `platform.h`.
     ```
   - **Publish release** klicken.

4. Der Workflow startet automatisch und hängt die Binaries an das Release an.

**Optional (GitHub CLI):** Mit `gh release create v1.0.2 --title "v1.0.2" --notes "- **macOS build:** …"` kannst du das Release von der Kommandozeile erstellen; die Notes sind die Release-Beschreibung (Changelog-Text).

Wenn ein Build fehlschlägt: Fix committen/pushen, dann Release löschen und mit gleichem Tag neu anlegen oder ein neues Patch-Release (z. B. v1.0.3) erstellen.
