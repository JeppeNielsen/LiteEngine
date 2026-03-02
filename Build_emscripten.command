#!/bin/bash

cd -- "$(dirname "$0")"

./premake/premake5 --file=Build.lua --emscripten gmake2

emmake make -C ./build/emscripten config=debug ExampleApplication
