workspace "CocoEngine"
    configurations { "Debug", "Release" }
    platforms { "Win64" }
    architecture "x64"

    startproject "CocoSandbox"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}\\"

-- Option for enabling SDL
--newoption {
--    trigger = "sdl-enabled",
--    description = "Include SDL in the engine build"
--}

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
--LibraryDir["sdl"] = "%{wks.location}\\vendor\\SDL2\\lib\\x64\\"

-- Output directories relative to the workspace
OutputDir = {}
OutputDir["bin"] = "%{wks.location}\\build\\bin\\" .. outputdir
OutputDir["obj"] = "%{wks.location}\\build\\obj\\" .. outputdir

-- Include directories relative to the workspace
IncludeDir = {}
IncludeDir["Coco"] = "%{wks.location}\\src\\"
--IncludeDir["imgui"] = "%{wks.location}\\vendor\\ImGui\\"

--filter "options:sdl-enabled"
--    IncludeDir["sdl"] = "%{wks.location}\\vendor\\SDL2\\include\\"

--filter "options:vulkan-enabled"
--    IncludeDir["volk"] = "%{wks.location}\\vendor\\volk\\"
--    IncludeDir["vulkan"] = _OPTIONS["vulkan-sdk-path"] .. "\\Include\\"

-- Deactivate the current filter
filter {}

--group "Dependencies"
--    include "NovaEngine\\vendor\\premakes\\ImGui"
    
group "Engine"
    include "src\\Coco"

group ""
    include "examples\\CocoSandbox"