project "Coco.Core"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    pchheader "Corepch.h"
    pchsource "Corepch.cpp"

    OutputBin = "%{OutputDir.bin}%{prj.name}"
    OutputObj = "%{OutputDir.obj}%{prj.name}"

    targetdir(OutputBin)
    objdir(OutputObj)

    defines { "COCO_ASSERTIONS" }

    -- Core files
    files
    {
        "**.h",
        "**.cpp",
    }

    includedirs
    {
        "%{IncludeDir.Coco}"
    }

    links
    {}
    
    -- Build configs
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