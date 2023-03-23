project "CocoSandbox"
    kind "WindowedApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

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
        "Coco",
    }

    filter "options:renderRHI-vulkan or options:renderRHIs-all"
        defines { "COCO_RENDERING_VULKAN"}

        libdirs
        {
            "%{LibraryDir.vulkan}",
        }

        links
        {
            "vulkan-1.lib"
        }

    filter {"system:windows", "options:renderRHI-vulkan or options:renderRHIs-all"}
        -- TEMPORARY
        postbuildcommands
        {
            "if not exist %{AssetsDir}shaders mkdir %{AssetsDir}shaders",
            "echo Compiling shaders...", 
            "echo %{AssetsDir}shaders\\built-in\\ObjectShader.vert.glsl to %{AssetsDir}shaders\\built-in\\ObjectShader.vert.spv",
            "%{BinDir.vulkan}\\glslc.exe -fshader-stage=vert %{AssetsDir}shaders\\built-in\\ObjectShader.vert.glsl -o %{AssetsDir}shaders\\built-in\\ObjectShader.vert.spv",
            "if %ERRORLEVEL% NEQ 0 (echo Error compiling vertex shader: %ERRORLEVEL% && exit)",
            "echo %{AssetsDir}shaders\\built-in\\ObjectShader.frag.glsl to %{AssetsDir}shaders\\built-in\\ObjectShader.frag.spv",
            "%{BinDir.vulkan}\\glslc.exe -fshader-stage=frag %{AssetsDir}shaders\\built-in\\ObjectShader.frag.glsl -o %{AssetsDir}shaders\\built-in\\ObjectShader.frag.spv",
            "if %ERRORLEVEL% NEQ 0 (echo Error compiling fragment shader: %ERRORLEVEL% && exit)",
            "echo Shaders compiled"
        }

    filter "configurations:Debug"
        defines
        { 
            "COCO_LOG_TRACE",
            "COCO_LOG_INFO",
            "COCO_LOG_WARNING",
        }

        debugdir "%{wks.location}"

        runtime "Debug"
        symbols "on"
        
    filter "configurations:Release"
        runtime "Release"
        optimize "on"