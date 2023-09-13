project "Sandbox"
    kind "WindowedApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    defines {"COCO_ASSERTIONS"}
    
    targetdir "%{OutputDir.bin}%{prj.name}"
    objdir "%{OutputDir.obj}%{prj.name}"

    TargetDir = "%{OutputDir.bin}%{prj.name}\\"

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
        "Coco.Core",
    }

    if (Services["Input"] == true) then
        links { "Coco.Input" }
    end

    if (Services["Rendering"] == true) then
        links { "Coco.Rendering" }

        if (RenderRHI["Vulkan"] == true) then
            libdirs
            {
                "%{LibraryDir.vulkan}"
            }
    
            links
            {
                "vulkan-1.lib"
            }
        end
    end

    if (Services["Windowing"] == true) then
        links { "Coco.Windowing" }
    end

    if (Platforms["Win32"] == true) then
        links { "Coco.Platforms.Win32" }
    end

    filter { "configurations:Debug" }
        defines
        { 
            "COCO_LOG_TRACE",
            "COCO_LOG_INFO",
            "COCO_LOG_WARNING",
        }

        debugargs { "--show-console" }

        runtime "Debug"
        symbols "on"
        
    filter { "configurations:Release" }
        runtime "Release"
        optimize "on"