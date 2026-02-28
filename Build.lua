require "clion"
local root = path.getabsolute(".")
local build_dir = path.join(root, "build")
local assets_dir = path.join(root, "src/Assets")

workspace "LiteEngine"
    location (build_dir)
    configurations { "Debug", "Release" }
    startproject "LiteEngine"

project "LiteEngine"
    kind "WindowedApp"
    language "C++"
    cppdialect "C++17"
    warnings "Extra"
    files {
        path.join(root, "src/**.mm"),
        path.join(root, "src/**.cpp"),
        path.join(root, "src/**.h"),
        path.join(root, "external/imgui/imgui*.cpp"),
    }
    includedirs {
        path.join(root, "external/sokol"),
        path.join(root, "external/sokol/util"),
        path.join(root, "external/imgui"),
        path.join(root, "external/glm"),
    }
    targetdir (path.join(build_dir, "bin/%{cfg.buildcfg}"))
    objdir (path.join(build_dir, "obj/%{cfg.buildcfg}"))
    if _ACTION == "clion" or _ACTION == "cmake" then
        local cmake_target_dir = "$<TARGET_FILE_DIR:LiteEngine>"
        postbuildcommands {
            "\"${CMAKE_COMMAND}\" -E make_directory \"" .. cmake_target_dir .. "/Assets\"",
            "\"${CMAKE_COMMAND}\" -E copy_directory \"" .. assets_dir .. "\" \"" .. cmake_target_dir .. "/Assets\"",
        }
    else
        postbuildcommands {
            "{MKDIR} \"%{cfg.targetdir}/Assets\"",
            "{COPYDIR} \"" .. assets_dir .. "\" \"%{cfg.targetdir}/Assets\"",
        }
    end

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
