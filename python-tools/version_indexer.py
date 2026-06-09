#!/usr/bin/env python3
"""Fetch Mojang's version manifest and emit a compact local index."""

from __future__ import annotations

import argparse
import json
import urllib.request
from pathlib import Path


MANIFEST_URL = "https://piston-meta.mojang.com/mc/game/version_manifest_v2.json"


def main() -> int:
    parser = argparse.ArgumentParser(description="Index Minecraft Java versions.")
    parser.add_argument("--output", type=Path, default=Path("version-index.json"))
    args = parser.parse_args()

    with urllib.request.urlopen(MANIFEST_URL, timeout=30) as response:
        manifest = json.loads(response.read().decode("utf-8"))

    versions = [
        {"id": item["id"], "type": item["type"], "url": item["url"]}
        for item in manifest.get("versions", [])
    ]
    args.output.write_text(json.dumps({"versions": versions}, indent=2) + "\n", encoding="utf-8")
    print(f"Indexed {len(versions)} versions into {args.output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
