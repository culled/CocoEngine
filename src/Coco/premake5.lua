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
    }

    links
    {}

    -- Engine service files
    filter { "options:service-input or options:services-all" }
        files
        {
            "Input/**.h",
            "Input/**.cpp",
        }

    filter { "options:service-rendering or options:services-all" }
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
            "Rendering/Loaders/**.h",
            "Rendering/Loaders/**.cpp",
        }

    filter { "options:service-windowing or options:service-rendering or options:services-all" }
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
            "Platforms/Windows/**.h",
            "Platforms/Windows/**.cpp",
        }

        systemversion "latest"

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