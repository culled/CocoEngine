workspace "NovaGameEngine"
    configurations { "Debug", "Release" }
    architecture "x64"

    startproject "ExampleApp"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}\\"

-- Option for enabling SDL
newoption {
    trigger = "sdl-enabled",
    description = "Include SDL in the engine build"
}

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

-- Option for enabling ImGui
newoption {
    trigger = "imgui-enabled",
    description = "Include ImGui in the engine build"
}

-- Library directories relative to the workspace
LibraryDir = {}
LibraryDir["sdl"] = "%{wks.location}\\NovaEngine\\vendor\\SDL2\\lib\\x64\\"

-- Output directories relative to the workspace
OutputDir = {}
OutputDir["bin"] = "%{wks.location}\\bin\\" .. outputdir
OutputDir["obj"] = "%{wks.location}\\obj\\" .. outputdir

-- Include directories relative to the workspace
IncludeDir = {}
IncludeDir["nova"] = "%{wks.location}\\NovaEngine\\src\\"
IncludeDir["imgui"] = "%{wks.location}\\NovaEngine\\vendor\\ImGui\\"

filter "options:sdl-enabled"
    IncludeDir["sdl"] = "%{wks.location}\\NovaEngine\\vendor\\SDL2\\include\\"

filter "options:vulkan-enabled"
    IncludeDir["volk"] = "%{wks.location}\\NovaEngine\\vendor\\volk\\"
    IncludeDir["vulkan"] = _OPTIONS["vulkan-sdk-path"] .. "\\Include\\"

-- Deactivate the current filter
filter {}

group "Dependencies"
    include "NovaEngine\\vendor\\premakes\\ImGui"
    
group ""
    include "NovaEngine"
    include "ExampleApp"