project "Coco.ImGui"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    OutputBin = "%{OutputDir.bin}Coco/%{prj.name}"
    OutputObj = "%{OutputDir.obj}Coco/%{prj.name}"

    targetdir(OutputBin)
    objdir(OutputObj)

    defines { "COCO_ASSERTIONS" }

    files
    {
        "**.h",
        "**.cpp",
        "%{IncludeDir.ImGui}\\*.h",
        "%{IncludeDir.ImGui}\\*.cpp",
        "%{IncludeDir.ImGuizmo}\\ImGuizmo.h",
        "%{IncludeDir.ImGuizmo}\\ImGuizmo.cpp"
    }

    includedirs
    {
        "%{IncludeDir.Coco}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.ImGuizmo}",
    }

    links
    {
        "Coco.Core",
        "Coco.Input",
        "Coco.Rendering",
        "Coco.Windowing"
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