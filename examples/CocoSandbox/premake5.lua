project "CocoSandbox"
    kind "WindowedApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    defines {"COCO_ASSERTIONS"}
    
    targetdir "%{OutputDir.bin}%{prj.name}"
    objdir "%{OutputDir.obj}%{prj.name}"

    files
    {
        "src/**.h",
        "src/**.cpp",
    }

    includedirs
    {
        "%{IncludeDir.Coco}",
    }

    links
    {
        "Coco",
        --"SDL2.lib",
    }

    libdirs
    {
        --"%{LibraryDir.sdl}",
    }

    filter "system:windows"
        postbuildcommands
        {
            --"xcopy /D /Y ..\\NovaEngine\\vendor\\SDL2\\lib\\x64\\SDL2.dll ..\\bin\\" .. outputdir .. "%{prj.name}\\SDL2.dll"
        }

    filter "configurations:Debug"
        defines
        { 
            "COCO_LOG_TRACE",
            "COCO_LOG_INFO",
            "COCO_LOG_WARNING",
        }

        runtime "Debug"
        symbols "on"
        
    filter "configurations:Release"
        runtime "Release"
        optimize "on"