#!/usr/bin/env python3
"""Mirror a list of asset URLs into a local directory."""

from __future__ import annotations

import argparse
import hashlib
import urllib.request
from pathlib import Path


def safe_name(url: str) -> str:
    suffix = Path(url.split("?", 1)[0]).suffix
    return hashlib.sha1(url.encode("utf-8")).hexdigest() + suffix


def main() -> int:
    parser = argparse.ArgumentParser(description="Mirror launcher assets.")
    parser.add_argument("urls", nargs="+", help="Asset URLs to download")
    parser.add_argument("--output-dir", type=Path, default=Path("asset-mirror"))
    args = parser.parse_args()

    args.output_dir.mkdir(parents=True, exist_ok=True)
    for url in args.urls:
        target = args.output_dir / safe_name(url)
        urllib.request.urlretrieve(url, target)
        print(f"{url} -> {target}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
