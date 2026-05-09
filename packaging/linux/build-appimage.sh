#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
PUBLISH_DIR="${1:-$ROOT_DIR/publish/linux-x64}"
APPDIR="$ROOT_DIR/AppDir"
DIST_DIR="$ROOT_DIR/dist"
APPIMAGETOOL_BIN="${APPIMAGETOOL:-appimagetool}"

if [[ ! -d "$PUBLISH_DIR" ]]; then
  echo "Publish directory not found: $PUBLISH_DIR" >&2
  exit 1
fi

rm -rf "$APPDIR"
mkdir -p "$APPDIR/usr/bin" \
         "$APPDIR/usr/share/applications" \
         "$APPDIR/usr/share/icons/hicolor/256x256/apps" \
         "$APPDIR/usr/share/metainfo" \
         "$DIST_DIR"

cp -R "$PUBLISH_DIR"/. "$APPDIR/usr/bin/"
install -m 755 "$ROOT_DIR/packaging/linux/AppRun" "$APPDIR/AppRun"
install -m 644 "$ROOT_DIR/packaging/linux/XylarJava.desktop" "$APPDIR/usr/share/applications/XylarJava.desktop"
install -m 644 "$ROOT_DIR/packaging/linux/XylarJava.desktop" "$APPDIR/XylarJava.desktop"
install -m 644 "$ROOT_DIR/packaging/linux/XylarJava.appdata.xml" "$APPDIR/usr/share/metainfo/XylarJava.appdata.xml"
install -m 644 "$ROOT_DIR/Assets/minecraft.png" "$APPDIR/usr/share/icons/hicolor/256x256/apps/XylarJava.png"
install -m 644 "$ROOT_DIR/Assets/minecraft.png" "$APPDIR/XylarJava.png"
install -m 644 "$ROOT_DIR/Assets/minecraft.png" "$APPDIR/.DirIcon"

chmod +x "$APPDIR/AppRun"
chmod +x "$APPDIR/usr/bin/XylarJavaLauncher" || true

APPIMAGE_EXTRACT_AND_RUN=1 "$APPIMAGETOOL_BIN" --no-appstream "$APPDIR" "$DIST_DIR/XylarJava-x86_64.AppImage"
