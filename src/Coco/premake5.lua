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
    }

    includedirs
    {
        "%{IncludeDir.Coco}",
        --"%{IncludeDir.sdl}",
    }

    links
    {}

    filter "options:sdl-enabled"
        defines { "COCO_PLATFORM_SDL"}

        files
        {}

        includedirs
        {
            "%{IncludeDir.volk}",
            "%{IncludeDir.vulkan}",
        }

    filter "options:vulkan-enabled"
        defines { "COCO_RENDERING_VULKAN"}

        files
        {}

        includedirs
        {}

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