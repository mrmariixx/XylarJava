#!/usr/bin/env bash
set -euo pipefail

export ARTIFACT_NAME='Linux-Qt6'
export BUILD_PLATFORM='official'
export JAVA_HOME=/usr/lib/jvm/java-17-openjdk-amd64
export PATH="$JAVA_HOME/bin:$PATH"

PROJECT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT"

rm -rf build install
cmake --preset linux -DCMAKE_BUILD_TYPE=Release
cmake --build --preset linux --config Release -j"$(nproc)"
cmake --install build --config Release

mkdir -p ~/bin
if [ ! -x ~/bin/mkappimage ]; then
  wget -q -O /tmp/mkappimage "https://github.com/DioEgizio/go-appimage/releases/download/continuous/mkappimage-continuous-x86_64.AppImage"
  chmod +x /tmp/mkappimage
  cp /tmp/mkappimage ~/bin/mkappimage
fi

INSTALL_APPIMAGE="$PROJECT/install-appimage"
rm -rf "$INSTALL_APPIMAGE"
cp -a "$PROJECT/install" "$INSTALL_APPIMAGE"
mkdir -p "$INSTALL_APPIMAGE/bin"
if [[ -f "$PROJECT/auth-settings.ini" ]]; then
  cp "$PROJECT/auth-settings.ini" "$INSTALL_APPIMAGE/auth-settings.ini"
  cp "$PROJECT/auth-settings.ini" "$INSTALL_APPIMAGE/bin/auth-settings.ini"
fi

OUT="$PROJECT/XylarJava-Linux-x86_64.AppImage"
~/bin/mkappimage --appdir "$INSTALL_APPIMAGE" --output "$OUT"
echo "AppImage: $OUT"
