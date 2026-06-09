# XylarJava

XylarJava is a modern Minecraft Java launcher built with C++17, CMake and Qt Widgets.

The launcher includes Mojang metadata fetching, local instances, library and asset downloads, native extraction, and offline launch command generation. The app icon is generated from the provided Minecraft WebP image and embedded as PNG/ICO resources.

## Project Layout

```text
XylarJava/
├── src/
│   ├── app/
│   ├── ui/
│   ├── launcher/
│   ├── minecraft/
│   ├── auth/
│   ├── instances/
│   ├── modpacks/
│   ├── download/
│   ├── java/
│   ├── net/
│   ├── fs/
│   └── util/
├── ui/
│   ├── qml/
│   └── widgets/
├── python-tools/
├── data/
│   ├── instances/
│   ├── libraries/
│   ├── assets/
│   └── runtimes/
├── tests/
├── CMakeLists.txt
└── README.md
```

## Build

Install Qt 6 or Qt 5 with the Widgets, Svg and Network modules, then run:

```powershell
cmake -S . -B build -G Ninja
cmake --build build
```

The executable is created as `build/XylarJava.exe` on Windows.

## Notes

- Navbar icons are SVG assets from Lucide Icons.
- `python-tools/version_indexer.py` can generate a compact Minecraft version index.
- The core launcher modules are intentionally small now, ready for Microsoft auth, Mojang metadata, Prism-style instances, modpack import/export and threaded downloads.
