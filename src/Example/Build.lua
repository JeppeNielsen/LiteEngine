require "clion"
local root = path.getabsolute(path.join(_SCRIPT_DIR, "..", ".."))
local build_dir = path.join(root, "build/example")
local assets_dir = path.join(root, "src/Example/Assets")

dofile(path.join(root, "premake", "LiteEngine.lua"))

workspace "ExampleApplication"
    location (build_dir)
    configurations { "Debug", "Release" }
    startproject "ExampleApplication"

liteengine_project(root, build_dir)

project "ExampleApplication"
    kind "WindowedApp"
    language "C++"
    cppdialect "C++20"
    warnings "Extra"
    files {
        path.join(root, "src/Example/**.mm"),
        path.join(root, "src/Example/**.cpp"),
        path.join(root, "src/Example/**.h"),
        path.join(root, "src/Example/**.hpp"),
    }
    includedirs {
        path.join(root, "src/Example"),
        path.join(root, "src"),
        path.join(root, "external/sokol"),
        path.join(root, "external/sokol/util"),
        path.join(root, "external/imgui"),
        path.join(root, "external/glm"),
        path.join(root, "external/entt/src"),
    }
    links { "LiteEngine" }
    targetdir (path.join(build_dir, "bin/%{cfg.buildcfg}"))
    objdir (path.join(build_dir, "obj/%{cfg.buildcfg}/%{prj.name}"))
    if _ACTION == "clion" or _ACTION == "cmake" then
        local cmake_target_dir = "$<TARGET_FILE_DIR:ExampleApplication>"
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
