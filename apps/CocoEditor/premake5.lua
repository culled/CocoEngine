project "CocoEditor"
    kind "WindowedApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    defines {"COCO_ASSERTIONS"}
    
    targetdir "%{OutputDir.bin}%{prj.name}"
    objdir "%{OutputDir.obj}%{prj.name}"

    TargetDir = "build\\bin\\" .. OutputFolder .. "\\%{prj.name}"

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
        "%{IncludeDir.entt}"
    }

    links 
    {
        "Coco.Core",
        "Coco.Input",
        "Coco.Rendering",
        "Coco.Windowing",
        "Coco.ImGui",
        "Coco.ECS",
        --"Coco.Physics3D",
    }

    if (Platforms["Win32"] == true) then
        links 
        {
            "Coco.Platforms.Win32",
        }
    end

    includedirs
    {
        "%{IncludeDir.Coco}",
    }

    filter { "system:windows" }
        postbuildcommands {
            "pushd %{wks.location}",
            "xcopy assets\\ %{TargetDir}\\assets\\ /S /Y /I",
            "popd"
        }

    filter { "configurations:Debug" }
        defines
        { 
            "COCO_LOG_TRACE",
            "COCO_LOG_INFO",
            "COCO_LOG_WARNING",
        }

        debugargs { "--show-console", "--content-path=%{wks.location}assets" }

        runtime "Debug"
        symbols "on"
        
    filter { "configurations:Release" }
        runtime "Release"
        optimize "on"