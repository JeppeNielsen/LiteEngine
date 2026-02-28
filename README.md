# LiteEngine (macOS)

A tiny standalone macOS app using the [Sokol](https://github.com/floooh/sokol) headers, built with C++ (Objective-C++) and premake.

## Prereqs
- `premake5` in PATH
- Xcode command line tools (for `xcodebuild` / clang)

## Sokol as a submodule (git external)
This repo expects Sokol at `external/sokol` as a git submodule.

If you have network access, run:

```bash
git submodule add https://github.com/floooh/sokol external/sokol
git submodule update --init --recursive
```

Expected headers:
- `external/sokol/sokol_app.h`
- `external/sokol/sokol_gfx.h`
- `external/sokol/sokol_glue.h`

## Build (premake + clang)
From the repo root:

```bash
premake5 xcode4
xcodebuild -project build/SokolStandalone.xcodeproj -configuration Debug
```

The app will be in:

```
build/bin/Debug/SokolStandalone.app
```

You can run it with:

```bash
open build/bin/Debug/SokolStandalone.app
```

## Notes
- The entry point is `src/main.mm` (Objective-C++). This is required because Sokol's macOS backend uses Cocoa APIs.
- The sample uses the OpenGL backend (`SOKOL_GLCORE`) for compatibility with older macOS SDKs.
