project "Coco.Platforms.Win32"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    pchheader "CPWinpch.h"
    pchsource "CPWinpch.cpp"

    OutputBin = "%{OutputDir.bin}%{prj.name}"
    OutputObj = "%{OutputDir.obj}%{prj.name}"

    targetdir(OutputBin)
    objdir(OutputObj)

    defines { "COCO_ASSERTIONS" }

    -- Win32 files
    files
    {
        "**.h",
        "**.cpp",
    }

    includedirs
    {
        "%{IncludeDir.Coco}",
    }

    links
    {
        "Coco.Core",
    }

    --if (Services["Windowing"]) then 
    --    links
    --    {
    --        "Coco.Windowing"
    --    }

    --    defines { "COCO_SERVICES_WINDOWING" }
    --else
    --    removefiles 
    --    {
    --        "Win32Window.h",
    --        "Win32Window.cpp",
    --    }
    --end

    --if (Services["Input"]) then 
    --    links
    --    {
    --        "Coco.Input"
    --    }
    --
    --    defines { "COCO_SERVICES_INPUT" }
    --end

    --if (Services["Rendering"]) then 
    --    links
    --    {
    --        "Coco.Rendering"
    --    }
--
    --    defines { "COCO_SERVICES_RENDERING" }
--
    --    if(RenderRHI["Vulkan"] == true) then
    --        defines { "COCO_RENDERING_VULKAN" }
    --    end
--
    --    if(RenderRHI["OpenGL"] == true) then
    --        defines { "COCO_RENDERING_OPENGL" }
    --    end
--
    --    if(RenderRHI["DX12"] == true) then
    --        defines { "COCO_RENDERING_DX12" }
    --    end
    --end

    if (_OPTIONS["high-dpi"] ~= nil) then
        defines { "COCO_HIGHDPI_SUPPORT" }
        links { "shcore.lib" }
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