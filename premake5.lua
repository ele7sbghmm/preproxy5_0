workspace "WORKSPACE"
    configurations { "Debug" }
    architecture "x86"
    location "build"
    -- systemversion "latest"
    cppdialect "C++17"
    -- staticruntime "On"

project "HOOK"
    kind "SharedLib"
    language "C++"
    targetdir "../SHAR"
    targetprefix ""
    targetname "d3d8"
    targetextension ".dll"

    files { "hook.cpp" }

    linkoptions {
        "../d3d8.def",
        -- "-Wl,--kill-at",
        "-static-libgcc",
        "-static-libstdc++",
        "-Wl,-Bstatic,--whole-archive",
        "-lwinpthread",
        "-Wl,--no-whole-archive",
        -- "-Wl,-Bdynamic",
    }

