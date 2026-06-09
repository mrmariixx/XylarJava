#!/usr/bin/env python3
"""Create a small modpack manifest scaffold for XylarJava."""

from __future__ import annotations

import argparse
import json
from pathlib import Path


def main() -> int:
    parser = argparse.ArgumentParser(description="Build a XylarJava modpack manifest scaffold.")
    parser.add_argument("name", help="Modpack display name")
    parser.add_argument("--minecraft", default="latest-release", help="Minecraft version id")
    parser.add_argument("--loader", default="vanilla", help="Loader name")
    parser.add_argument("--output", type=Path, default=Path("modpack.json"), help="Output manifest path")
    args = parser.parse_args()

    manifest = {
        "name": args.name,
        "minecraft": args.minecraft,
        "loader": args.loader,
        "files": [],
        "overrides": "overrides",
    }
    args.output.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    print(f"Wrote {args.output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
