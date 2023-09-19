project "Coco.Rendering"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    pchheader "Renderpch.h"
    pchsource "Renderpch.cpp"

    OutputBin = "%{OutputDir.bin}%{prj.name}"
    OutputObj = "%{OutputDir.obj}%{prj.name}"

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
        "%{IncludeDir.stb}"
    }

    links
    {
        "Coco.Core"
    }

    if (RenderRHI["Vulkan"] == true) then
        defines { "COCO_RENDERING_VULKAN" }

        includedirs
        {
            "%{IncludeDir.vulkan}"
        }

        libdirs
        {
            "%{LibraryDir.vulkan}"
        }

        links
        {
            "vulkan-1.lib"
        }
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