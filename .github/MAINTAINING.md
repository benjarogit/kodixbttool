# Maintainer notes (kodixbttool)

Kurze Notizen für dich als Maintainer – nicht für Endnutzer gedacht.

## Release-Workflow (CI)

Wenn du auf GitHub ein **Release veröffentlichst** (oder unter *Actions → Build release → Run workflow* manuell startest), läuft der Workflow **Build release**:

1. **Build-Job:** Pro Plattform (Linux, macOS, Windows) werden Repo ausgecheckt, Abhängigkeiten installiert (apt / Homebrew / vcpkg), CMake ausgeführt und das Binary gebaut.
2. **Artifacts:** Jedes Binary wird als Artifact hochgeladen (`kodixbttool-linux-x64`, `kodixbttool-macos`, `kodixbttool-windows-x64.exe`).
3. **Release-Assets:** Ein zweiter Job lädt alle Artifacts herunter, benennt sie um und hängt sie mit **softprops/action-gh-release** an **dasselbe Release** (an den Release-Tag) an. Auf der Release-Seite stehen dann Quellcode-Zip und diese Binaries zum Download.

Fertige Binaries entstehen also automatisch beim Veröffentlichen eines Releases; manueller Build oder Upload ist nicht nötig.

## Neues Release erstellen

1. Änderungen im **offiziellen Repo** haben (z. B. `~/Dokumente/kodixbttool` oder wo du pushst).
2. **CHANGELOG.md** anpassen: Einträge unter `[Unreleased]` in einen neuen Versionsblock z. B. `## [1.0.1] - YYYY-MM-DD` verschieben.
3. Committen, pushen:
   ```bash
   git add -A && git commit -m "Release v1.0.1: macOS build fix, workflow improvements"
   git push origin master
   ```
4. Auf GitHub: **Releases → Draft a new release**:
   - Tag: z. B. `v1.0.1` (neu erstellen).
   - Title: z. B. `v1.0.1`.
   - Beschreibung: Inhalt aus CHANGELOG für diese Version einfügen.
   - **Publish release** klicken.
5. Der Workflow startet automatisch, baut alle Plattformen und hängt die Binaries an das neue Release an.

Wenn ein Build (z. B. macOS oder Windows) fehlschlägt: Actions-Log prüfen, Fix im Repo machen, dann entweder das Release löschen und mit gleichem Tag neu anlegen oder ein Patch-Release (z. B. v1.0.2) machen.
