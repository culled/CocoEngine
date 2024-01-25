project "Coco.Physics3D"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    pchheader "Phys3Dpch.h"
    pchsource "Phys3Dpch.cpp"

    OutputBin = "%{OutputDir.bin}Coco/%{prj.name}"
    OutputObj = "%{OutputDir.obj}Coco/%{prj.name}"

    targetdir(OutputBin)
    objdir(OutputObj)

    defines { "COCO_ASSERTIONS" }

    files
    {
        "**.h",
        "**.cpp",
    }

    includedirs
    {
        "%{IncludeDir.Coco}",
        "%{IncludeDir.bullet}"
    }

    links
    {
        "Coco.Core",
        "bullet"
    }

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