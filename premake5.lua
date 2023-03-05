workspace "CocoEngine"
    configurations { "Debug", "Release" }
    platforms { "Win64" }
    architecture "x64"

    startproject "CocoSandbox"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}\\"

-- Option for enabling Vulkan
newoption {
    trigger = "vulkan-enabled",
    description = "Include Vulkan in the engine build"
}

-- Option for specifying Vulkan SDK path
newoption {
    trigger = "vulkan-sdk-path",
    value = "path",
    description = "Path to the Vulkan SDK"
}

-- Library directories relative to the workspace
LibraryDir = {}

-- Output directories relative to the workspace
OutputDir = {}
OutputDir["bin"] = "%{wks.location}\\build\\bin\\" .. outputdir
OutputDir["obj"] = "%{wks.location}\\build\\obj\\" .. outputdir

-- Include directories relative to the workspace
IncludeDir = {}
IncludeDir["Coco"] = "%{wks.location}\\src\\"
--IncludeDir["imgui"] = "%{wks.location}\\vendor\\ImGui\\"

-- Assets directory
AssetsDir = "%{wks.location}\\assets\\"

-- External program directories
BinDir = {}

filter "options:vulkan-enabled"
    IncludeDir["vulkan"] = _OPTIONS["vulkan-sdk-path"] .. "\\Include\\"
    LibraryDir["vulkan"] = _OPTIONS["vulkan-sdk-path"] .. "\\Lib\\"
    BinDir["vulkan"] = _OPTIONS["vulkan-sdk-path"] .. "\\Bin\\"

-- Deactivate the current filter
filter {}

--group "Dependencies"
--    include "NovaEngine\\vendor\\premakes\\ImGui"
    
group "Engine"
    include "src\\Coco"

group ""
    include "examples\\CocoSandbox"