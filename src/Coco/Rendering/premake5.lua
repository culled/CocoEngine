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

    defines { "COCO_ASSERTIONS", "YAML_CPP_STATIC_DEFINE" }

    files
    {
        "**.h",
        "**.cpp",
    }

    includedirs
    {
        "%{IncludeDir.Coco}",
        "%{IncludeDir.stb}",
        "%{IncludeDir.yaml_cpp}"
    }

    links
    {
        "Coco.Core",
        "yaml-cpp"
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

        if (RenderRHI["Vulkan"] == true) then
            links
            {
                "shaderc_sharedd.lib",
                "spirv-cross-cored.lib",
                "spirv-cross-glsld.lib",
                "SPIRV-Toolsd.lib"
            }
        end

        runtime "Debug"
        symbols "on"
        
    filter { "configurations:Release" }
        if (RenderRHI["Vulkan"] == true) then
            links
            {
                "shaderc_shared.lib",
                "spirv-cross-core.lib",
                "spirv-cross-glsl.lib"
            }
        end
        
        runtime "Release"
        optimize "on"