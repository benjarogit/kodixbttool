# Maintainer notes (kodixbttool)

Kurze Notizen für dich als Maintainer – nicht für Endnutzer gedacht.

## Release-Workflow (CI)

Wenn du auf GitHub ein **Release veröffentlichst** (oder unter *Actions → Build release → Run workflow* manuell startest), läuft der Workflow **Build release**:

1. **Build-Job:** Pro Plattform (Linux, macOS, Windows) werden Repo ausgecheckt, Abhängigkeiten installiert (apt / Homebrew / vcpkg), CMake ausgeführt und das Binary gebaut.
2. **Artifacts:** Jedes Binary wird als Artifact hochgeladen (`kodixbttool-linux-x64`, `kodixbttool-macos`, `kodixbttool-windows-x64.exe`).
3. **Release-Assets:** Ein zweiter Job lädt alle Artifacts herunter, benennt sie um und hängt sie mit **softprops/action-gh-release** an **dasselbe Release** (an den Release-Tag) an. Auf der Release-Seite stehen dann Quellcode-Zip und diese Binaries zum Download.

Fertige Binaries entstehen also automatisch beim Veröffentlichen eines Releases; manueller Build oder Upload ist nicht nötig.

## Neues Release erstellen (immer mit passendem Changelog)

**Wichtig:** Ein `git push` erstellt **kein** Release. Du musst das Release (Tag + Beschreibung) separat anlegen – per GitHub-Web oder mit dem Skript unten.

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

3. **Release anlegen** (nur so erscheint es unter Releases und der Build-Workflow startet):

   **Variante A – Skript (nur für Maintainer, Changelog aus CHANGELOG.md):**
   ```bash
   ./.github/scripts/release.sh 1.0.2
   ```
   Voraussetzung: [GitHub CLI](https://cli.github.com/) installiert und eingeloggt (`gh auth login`). Das Skript liegt unter `.github/scripts/` und ist nicht für Endnutzer gedacht.

   **Variante B – GitHub-Web:**
   - **Releases → Draft a new release**
   - **Choose a tag:** z. B. `v1.0.2` → **Create new tag: v1.0.2**
   - **Release title:** z. B. `v1.0.2`
   - **Describe this release:** Changelog-Abschnitt für diese Version aus CHANGELOG.md einfügen (die Punkte unter `## [1.0.2]`).
   - **Publish release** klicken.

4. Der Workflow startet automatisch; es laufen **3 Matrix-Jobs** (linux-x64, macos, windows-x64). **Hinweis:** Beim Run [v1.0.0 #1](https://github.com/benjarogit/kodixbttool/actions/runs/22074034285) siehst du nur 2 Jobs, weil dieser Run die Workflow-Datei **vom Tag v1.0.0** verwendet (ohne Windows). Ein **neu angelegtes** Release (z. B. v1.0.2) nutzt die aktuelle Datei von `master` und zeigt dann 3 Jobs.

Wenn ein Build fehlschlägt: Fix committen/pushen, Release löschen und mit gleichem Tag neu anlegen oder ein neues Patch-Release (z. B. v1.0.3) erstellen.
