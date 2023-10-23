project "Coco.ECS"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    pchheader "ECSpch.h"
    pchsource "ECSpch.cpp"

    OutputBin = "%{OutputDir.bin}%{prj.name}"
    OutputObj = "%{OutputDir.obj}%{prj.name}"

    targetdir(OutputBin)
    objdir(OutputObj)

    defines { "COCO_ASSERTIONS", "YAML_CPP_STATIC_DEFINE" }

    files
    {
        "**.h",
        "**.cpp",
    }

    includedirs
    {
        "%{IncludeDir.Coco}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.yaml_cpp}"
    }

    links
    {
        "Coco.Core",
        "yaml-cpp"
    }

    if(Services["Rendering"] ~= nil) then
        links
        {
            "Coco.Rendering"
        }

        defines { "COCO_SERVICE_RENDERING" }
    end

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