project "Coco.Tests"
    kind "SharedLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    defines {"COCO_ASSERTIONS"}
    
    targetdir "%{OutputDir.bin}%{prj.name}"
    objdir "%{OutputDir.obj}%{prj.name}"

    TargetDir = "%{OutputDir.bin}%{prj.name}\\"

    files
    {
        "**.h",
        "**.cpp",
    }

    includedirs
    {
        "src",
        "%{IncludeDir.Coco}",
    }

    links
    {
        "Coco",
    }

    filter { "configurations:Debug" }
        defines
        { 
            "COCO_LOG_TRACE",
            "COCO_LOG_INFO",
            "COCO_LOG_WARNING",
        }

        runtime "Debug"
        symbols "on"
        
    filter { "configurations:Release" }
        runtime "Release"
        optimize "on"