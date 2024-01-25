project "Coco.ECS"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    pchheader "ECSpch.h"
    pchsource "ECSpch.cpp"

    OutputBin = "%{OutputDir.bin}Coco/%{prj.name}"
    OutputObj = "%{OutputDir.obj}Coco/%{prj.name}"

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
        "yaml"
    }

    if(Services["Rendering"] ~= nil) then
        links
        {
            "Coco.Rendering"
        }

        defines { "COCO_SERVICE_RENDERING" }
    else
        removefiles
        {
            "Components/Rendering/**.h",
            "Components/Rendering/**.cpp",
            "Serializers/Components/Rendering/**.h",
            "Serializers/Components/Rendering/**.cpp",
            "Providers/Rendering/**.h",
            "Providers/Rendering/**.cpp",
        }
    end

--    if(Services["Physics3D"] ~= nil) then
--        links
--        {
--            "Coco.Physics3D"
--        }
--
--        defines { "COCO_SERVICE_PHYSICS3D" }
--    else
--        removefiles
--        {
--            "Components/Physics3D/**.h",
--            "Components/Physics3D/**.cpp",
--        }
--    end

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