#!/bin/bash

cd -- "$(dirname "$0")"

./premake/premake5 --file=Build.lua clion
