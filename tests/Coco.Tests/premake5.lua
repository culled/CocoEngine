project "Coco.Tests"
    kind "SharedLib"
    language "C++"
    cppdialect "C++20"

    defines {"COCO_ASSERTIONS"}
    
    targetdir "%{OutputDir.bin}%{prj.name}"
    objdir "%{OutputDir.obj}%{prj.name}"

    TargetDir = "%{OutputDir.bin}%{prj.name}\\"

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
        "Coco",
    }