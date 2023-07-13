project "Coco"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir "%{OutputDir.bin}%{prj.name}"
    objdir "%{OutputDir.obj}%{prj.name}"

    --defines { "COCO_EXPORT" }
    defines { "COCO_ASSERTIONS" }

    -- Core files
    files
    {
        "Core/**.h",
        "Core/**.cpp",
        "Vendor/**.h",
        "Vendor/**.cpp",
    }

    includedirs
    {
        "%{IncludeDir.Coco}",
        "%{IncludeDir.stb}",
        "%{IncludeDir.uuid_v4}",
    }

    links
    {}

    -- Engine service files
    filter { "options:service-input or options:services-all" }
        defines { "COCO_SERVICE_INPUT"}

        files
        {
            "Input/**.h",
            "Input/**.cpp",
        }

    filter { "options:service-ecs or options:services-all" }
        defines { "COCO_SERVICE_ECS"}

        files
        {
            "ECS/**.h",
            "ECS/**.cpp",
        }

    filter { "options:service-rendering or options:services-all" }
        defines { "COCO_SERVICE_RENDERING"}

        files
        {
            "Rendering/*.h",
            "Rendering/*.cpp",
            "Rendering/Components/**.h",
            "Rendering/Components/**.cpp",
            "Rendering/Graphics/*.h",
            "Rendering/Graphics/*.cpp",
            "Rendering/Graphics/Resources/**.h",
            "Rendering/Graphics/Resources/**.cpp",
            "Rendering/Pipeline/**.h",
            "Rendering/Pipeline/**.cpp",
            "Rendering/Serializers/**.h",
            "Rendering/Serializers/**.cpp",
        }

    filter { "options:service-windowing or options:service-rendering or options:services-all" }
        defines { "COCO_SERVICE_WINDOWING"}

        files
        {
            "Windowing/**.h",
            "Windowing/**.cpp",
        } 
    
    -- Render RHI files
    filter { "options:renderRHI-vulkan or options:renderRHIs-all", "options:service-rendering or options:services-all" }
        defines { "COCO_RENDERING_VULKAN"}

        files
        {
            "Rendering/Graphics/RHI/Vulkan/**.h",
            "Rendering/Graphics/RHI/Vulkan/**.cpp",
        }

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
    
    filter { "options:renderRHI-opengl or options:renderRHIs-all", "options:service-rendering or options:services-all" }
        defines { "COCO_RENDERING_OPENGL"}

        files
        {
        }

        includedirs
        {
        }

        libdirs
        {
        }

        links
        {
        }
    
    filter { "options:renderRHI-dx12 or options:renderRHIs-all", "options:service-rendering or options:services-all" }
        defines { "COCO_RENDERING_DX12"}

        files
        {
        }

        includedirs
        {
        }

        libdirs
        {
        }

        links
        {
        }

    -- Engine platform files
    filter { "options:platform-windows or options:platforms-all" }
        files
        {
            "Platforms/Windows/APIWindows.h",
            "Platforms/Windows/EnginePlatformWindows.h",
            "Platforms/Windows/EnginePlatformWindows.cpp",
            "Platforms/Windows/EntryWindows.cpp",
            "Platforms/Windows/WindowsIncludes.h",
        }

        systemversion "latest"

    filter { "options:platform-windows or options:platforms-all", "options:service-windowing or options:services-all" }
        files
        {
            "Platforms/Windows/WindowsWindow.h",
            "Platforms/Windows/WindowsWindow.cpp",
        }

    filter { "options:platform-mac or options:platforms-all" }
        files
        {
        }

    filter { "options:platform-linux or options:platforms-all" }
        files
        {
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