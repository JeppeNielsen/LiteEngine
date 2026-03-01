function liteengine_project(root, build_dir)
    project "LiteEngine"
        kind "StaticLib"
        language "C++"
        cppdialect "C++20"
        warnings "Extra"
        files {
            path.join(root, "src/Lite/**.mm"),
            path.join(root, "src/Lite/**.cpp"),
            path.join(root, "src/Lite/**.h"),
            path.join(root, "src/Lite/**.hpp"),
            path.join(root, "external/imgui/imgui*.cpp"),
        }
        includedirs {
            path.join(root, "src"),
            path.join(root, "external/sokol"),
            path.join(root, "external/sokol/util"),
            path.join(root, "external/imgui"),
            path.join(root, "external/glm"),
            path.join(root, "external/entt/src"),
        }
        targetdir (path.join(build_dir, "bin/%{cfg.buildcfg}"))
        objdir (path.join(build_dir, "obj/%{cfg.buildcfg}/%{prj.name}"))
        filter "configurations:Debug"
            symbols "On"
        filter "configurations:Release"
            optimize "On"
        filter {}
end
