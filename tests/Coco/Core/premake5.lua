project "Coco.Core.Tests"
    kind "SharedLib"
    language "C++"
    cppdialect "C++20"

    pchheader "pch.h"
    pchsource "pch.cpp"
    
    OutputBin = "%{OutputDir.bin}Tests/%{prj.name}"
    OutputObj = "%{OutputDir.obj}Tests/%{prj.name}"

    targetdir(OutputBin)
    objdir(OutputObj)

    defines {"COCO_ASSERTIONS"}

    files
    {
        "**.h",
        "**.cpp",
    }

    includedirs
    {
        "src",
        "%{IncludeDir.Coco}",
    }

    links
    {
        "Coco.Core",
    }

    filter { "platforms:Static" }
        staticruntime "on"

    filter { "platforms:DLL" }
        staticruntime "off"
        defines { "COCO_DLL" }

        postbuildcommands
        {
            "xcopy /y /d %{OutputDir.bin}\\Coco.Core\\*.dll %{OutputBin}\\"
        }

    filter { "configurations:Debug" }
        defines
        { 
            "COCO_LOG_TRACE",
            "COCO_LOG_INFO",
            "COCO_LOG_WARNING",
        }

        runtime "Debug"
        symbols "on"

        --postbuildcommands
        --{
        --    "xcopy %{prj.location}files\\ %{TargetDir}files\\ /S /Y",
        --}
        
    filter { "configurations:Release" }
        runtime "Release"
        optimize "on"