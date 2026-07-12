#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${1:-${ROOT}/build-win}"

if [[ ! -f "${BUILD_DIR}/CMakeCache.txt" ]]; then
    echo "Build directory not configured: ${BUILD_DIR}" >&2
    exit 1
fi

echo "Deploying runtime to ${BUILD_DIR}..."
cmake --install "${BUILD_DIR}" --component Runtime
cmake --install "${BUILD_DIR}" --component bundle

AUTH_SETTINGS="${ROOT}/auth-settings.ini"
AUTH_EXAMPLE="${ROOT}/auth-settings.ini.example"
if [[ -f "${AUTH_SETTINGS}" ]]; then
    cp "${AUTH_SETTINGS}" "${BUILD_DIR}/auth-settings.ini"
    echo "Copied auth-settings.ini into portable bundle"
elif [[ -f "${AUTH_EXAMPLE}" ]]; then
    cp "${AUTH_EXAMPLE}" "${BUILD_DIR}/auth-settings.ini.example"
    echo "Copied auth-settings.ini.example into portable bundle"
fi

echo "Done. Portable files are in: ${BUILD_DIR}"
echo "  XylarJava.exe + $(find "${BUILD_DIR}" -maxdepth 1 -name '*.dll' | wc -l) DLLs"
if [[ -d "${BUILD_DIR}/platforms" ]]; then
    echo "  Qt plugins: platforms/, imageformats/, styles/"
fi
