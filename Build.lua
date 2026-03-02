require "clion"
newoption {
    trigger = "emscripten",
    description = "Build with Emscripten (WebGL2)",
}
local root = path.getabsolute(".")
local build_dir = path.join(root, "build")
if _OPTIONS["emscripten"] then
    build_dir = path.join(root, "build", "emscripten")
end
local assets_dir = path.join(root, "src/Example/Assets")
local emscripten_shell = path.join(root, "src/Example/emscripten_shell.html")

dofile(path.join(root, "premake", "LiteEngine.lua"))

workspace "LiteEngine"
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
        emscripten_shell,
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
    filter "options:emscripten"
        targetextension ".html"
        removelinkoptions {
            "-framework Cocoa",
            "-framework OpenGL",
        }
        linkoptions {
            "-s USE_WEBGL2=1",
            "-s FULL_ES3=1",
            "-s MIN_WEBGL_VERSION=2",
            "-s MAX_WEBGL_VERSION=2",
            "-s ALLOW_MEMORY_GROWTH=1",
            "-s STACK_SIZE=2097152",
            "--preload-file " .. assets_dir .. "@/Assets",
            "--shell-file " .. emscripten_shell,
        }
    filter { "options:emscripten", "configurations:Debug" }
        buildoptions {
            "-g",
        }
        linkoptions {
            "-g",
            "-gsource-map",
            "--profiling-funcs",
            "-s ASSERTIONS=2",
            "-s SAFE_HEAP=1",
            "-s STACK_OVERFLOW_CHECK=2",
        }
    filter "configurations:Debug"
        symbols "On"
    filter "configurations:Release"
        optimize "On"
    filter {}
