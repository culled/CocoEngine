project "Coco"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir "%{OutputDir.bin}%{prj.name}"
    objdir "%{OutputDir.obj}%{prj.name}"

    --defines { "COCO_EXPORT" }
    defines { "COCO_ASSERTIONS" }

    files
    {
        "Core/**.h",
        "Core/**.cpp",
        "Windowing/**.h",
        "Windowing/**.cpp",
        "Input/**.h",
        "Input/**.cpp",
        "Vendor/**.h",
        "Vendor/**.cpp",
        "Rendering/*.h",
        "Rendering/*.cpp",
        "Rendering/Components/**.h",
        "Rendering/Components/**.cpp",
        "Rendering/Graphics/*.h",
        "Rendering/Graphics/*.cpp",
        "Rendering/Pipeline/**.h",
        "Rendering/Pipeline/**.cpp",
    }

    includedirs
    {
        "%{IncludeDir.Coco}",
        "%{IncludeDir.stb}",
    }

    links
    {}
    
    filter "options:vulkan-enabled"
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

    filter "system:windows"
        files
        {
            "Platforms/Windows/**.h",
            "Platforms/Windows/**.cpp",
        }

        systemversion "latest"

    filter "configurations:Debug"
        defines
        { 
            "COCO_LOG_TRACE",
            "COCO_LOG_INFO",
            "COCO_LOG_WARNING",
        }

        runtime "Debug"
        symbols "on"
        
    filter "configurations:Release"
        runtime "Release"
        optimize "on"