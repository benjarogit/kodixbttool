#!/usr/bin/env bash
# Maintainer only: create a GitHub release with tag vX.Y.Z and notes from CHANGELOG.md.
# Usage: ./.github/scripts/release.sh 1.0.2   (from repo root)
# Requires: gh (GitHub CLI), logged in (gh auth status).

set -e
V="$1"
if [ -z "$V" ]; then
  echo "Usage: $0 <version>" >&2
  echo "Example: $0 1.0.2" >&2
  exit 1
fi

TAG="v${V}"
CHANGELOG="CHANGELOG.md"
if [ ! -f "$CHANGELOG" ]; then
  echo "CHANGELOG.md not found. Run from repo root." >&2
  exit 1
fi

# Extract lines between "## [X.Y.Z]" and the next "---"
NOTES=$(grep -A 200 "## \[$V\]" "$CHANGELOG" | sed '/^---$/q' | tail -n +2 | grep -v '^---$' | sed 's/^[[:space:]]*//')
if [ -z "$NOTES" ]; then
  echo "No section for [$V] in CHANGELOG.md" >&2
  exit 1
fi

echo "Creating release $TAG with notes:"
echo "$NOTES"
echo "---"
if ! command -v gh &>/dev/null; then
  echo "Install gh (GitHub CLI) and run: gh release create $TAG --title $TAG --notes \"$NOTES\""
  exit 1
fi

gh release create "$TAG" --title "$TAG" --notes "$NOTES"
