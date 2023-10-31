project "CocoEditor"
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
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.ImGuizmo}",
        "%{IncludeDir.yaml_cpp}",
        "%{IncludeDir.entt}"
    }

    links
    {
        "Coco.Core",
        "Coco.Input",
        "Coco.Rendering",
        "Coco.Windowing",
        "Coco.ImGui",
        "Coco.ECS"
    }

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

    if (Platforms["Win32"] == true) then
        links { "Coco.Platforms.Win32" }
    end

    filter { "system:windows", "options:renderRHI-vulkan or options:renderRHIs-all" }
        postbuildcommands {
            "xcopy %{AssetsDir} %{TargetDir}assets\\ /S /Y /I"
        }

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

        debugargs { "--show-console", "--content-path=%{wks.location}assets" }

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