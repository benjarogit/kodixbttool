#!/usr/bin/env bash
# Run kodixbttool tests: build check, extract, print, pack, dupecheck, help.
# Set KODIXBT_TEST_XBT_DIR to a directory containing .xbt files (e.g. skin media folder).
# If unset: tries default ~/.kodi/addons/skin.dokukanal/media/ then ./testdata/

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

BIN="${KODIXBT_BIN:-./kodixbttool}"
if [[ ! -x "$BIN" ]]; then
  echo "Binary not found or not executable: $BIN (set KODIXBT_BIN or run from tools/kodixbttool after make)"
  exit 1
fi

# Directory containing at least one .xbt for extract/print tests
if [[ -n "$KODIXBT_TEST_XBT_DIR" ]]; then
  XBT_DIR="$KODIXBT_TEST_XBT_DIR"
elif [[ -d "$HOME/.kodi/addons/skin.dokukanal/media" ]]; then
  XBT_DIR="$HOME/.kodi/addons/skin.dokukanal/media"
elif [[ -d "$SCRIPT_DIR/testdata" ]]; then
  XBT_DIR="$SCRIPT_DIR/testdata"
else
  echo "No .xbt directory found. Set KODIXBT_TEST_XBT_DIR or add .xbt files to testdata/"
  exit 1
fi

# Find first .xbt in directory
XBT_FILE=""
for f in "$XBT_DIR"/*.xbt; do
  if [[ -f "$f" ]]; then
    XBT_FILE="$f"
    break
  fi
done
if [[ -z "$XBT_FILE" ]]; then
  echo "No .xbt file in $XBT_DIR"
  exit 1
fi

TMP_DIR=""
cleanup() {
  if [[ -n "$TMP_DIR" && -d "$TMP_DIR" ]]; then
    rm -rf "$TMP_DIR"
  fi
}
trap cleanup EXIT
TMP_DIR="$(mktemp -d)"

echo "=== Help ==="
"$BIN" -h
echo ""

echo "=== Print paths ($XBT_FILE) ==="
"$BIN" -p "$XBT_FILE"
echo ""

echo "=== Extract all (create dirs) ==="
mkdir -p "$TMP_DIR/extracted"
"$BIN" -o "$TMP_DIR/extracted" -c "$XBT_FILE"
# ensure base dir exists for createPath (tool creates subdirs like common/, diffuse/)
EXTRACTED_COUNT=$(find "$TMP_DIR/extracted" -type f -name '*.png' -o -name '*.jpg' 2>/dev/null | wc -l)
echo "Extracted $EXTRACTED_COUNT image files."
[[ "$EXTRACTED_COUNT" -gt 0 ]] || { echo "No extracted images found."; exit 1; }
echo ""

echo "=== Pack (no-remove-input) ==="
"$BIN" --pack -i "$TMP_DIR/extracted" -o "$TMP_DIR/repack.xbt" --no-remove-input
[[ -f "$TMP_DIR/repack.xbt" ]] || { echo "repack.xbt not created"; exit 1; }
echo ""

echo "=== Pack with dupecheck ==="
"$BIN" --pack -i "$TMP_DIR/extracted" -o "$TMP_DIR/repack2.xbt" --dupecheck --no-remove-input
[[ -f "$TMP_DIR/repack2.xbt" ]] || { echo "repack2.xbt not created"; exit 1; }
echo ""

echo "All tests passed."
