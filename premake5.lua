require "clion"
local root = path.getabsolute(".")
local build_dir = path.join(root, "build")

workspace "SokolStandalone"
    location (build_dir)
    configurations { "Debug", "Release" }
    startproject "SokolStandalone"

project "SokolStandalone"
    kind "WindowedApp"
    language "C++"
    cppdialect "C++17"
    warnings "Extra"
    files {
        path.join(root, "src/**.mm"),
        path.join(root, "src/**.cpp"),
        path.join(root, "src/**.h"),
    }
    includedirs {
        path.join(root, "external/sokol"),
    }
    targetdir (path.join(build_dir, "bin/%{cfg.buildcfg}"))
    objdir (path.join(build_dir, "obj/%{cfg.buildcfg}"))

    filter "system:macosx"
        linkoptions {
            "-framework Cocoa",
            "-framework OpenGL",
        }
    filter "configurations:Debug"
        symbols "On"
    filter "configurations:Release"
        optimize "On"
    filter {}
