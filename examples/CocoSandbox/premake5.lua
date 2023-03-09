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

    filter "options:vulkan-enabled"
        defines { "COCO_RENDERING_VULKAN"}

        libdirs
        {
            "%{LibraryDir.vulkan}",
        }

        links
        {
            "vulkan-1.lib"
        }

    filter "system:windows"
        postbuildcommands
        {
            "echo Copying assets to %{TargetDir}assets...", 
            "xcopy %{AssetsDir} %{TargetDir}assets /h /i /c /k /e /r /y",
            "echo Assets copied"
        }  

    filter {"system:windows", "options:vulkan-enabled"}
        -- TEMPORARY
        postbuildcommands
        {
            "if not exist %{TargetDir}assets\\shaders mkdir %{TargetDir}assets\\shaders",
            "echo Compiling shaders...", 
            "echo %{TargetDir}assets\\shaders\\built-in\\ObjectShader.vert.glsl to %{TargetDir}assets\\shaders\\built-in\\ObjectShader.vert.spv",
            "%{BinDir.vulkan}\\glslc.exe -fshader-stage=vert %{TargetDir}assets\\shaders\\built-in\\ObjectShader.vert.glsl -o %{TargetDir}assets\\shaders\\built-in\\ObjectShader.vert.spv",
            "if %ERRORLEVEL% NEQ 0 (echo Error compiling vertex shader: %ERRORLEVEL% && exit)",
            "echo %{TargetDir}assets\\shaders\\built-in\\ObjectShader.frag.glsl to %{TargetDir}assets\\shaders\\built-in\\ObjectShader.frag.spv",
            "%{BinDir.vulkan}\\glslc.exe -fshader-stage=frag %{TargetDir}assets\\shaders\\built-in\\ObjectShader.frag.glsl -o %{TargetDir}assets\\shaders\\built-in\\ObjectShader.frag.spv",
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

        debugdir "%{OutputDir.bin}%{prj.name}\\"

        runtime "Debug"
        symbols "on"
        
    filter "configurations:Release"
        runtime "Release"
        optimize "on"