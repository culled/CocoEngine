workspace "CocoEngine"
    configurations { "Debug", "Release" }
    platforms { "Win64" }
    architecture "x64"

    startproject "CocoSandbox"

OutputFolder = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}\\"

-- Library directories relative to the workspace
LibraryDir = {}

-- Output directories relative to the workspace
OutputDir = {}
OutputDir["bin"] = "%{wks.location}\\build\\bin\\" .. OutputFolder
OutputDir["obj"] = "%{wks.location}\\build\\obj\\" .. OutputFolder

-- Include directories relative to the workspace
IncludeDir = {}
IncludeDir["Coco"] = "%{wks.location}\\src\\"
IncludeDir["stb"] = "%{wks.location}\\src\\Vendor\\stb"

-- Assets directory
AssetsDir = "%{wks.location}\\assets\\"

-- External program directories
BinDir = {}

-- Service includes
newoption {
    trigger = "service-input",
    description = "Include the input service in the engine build"
}

newoption {
    trigger = "service-rendering",
    description = "Include the rendering service in the engine build"
}

newoption {
    trigger = "service-windowing",
    description = "Include the windowing service in the engine build (will include the rendering service)"
}

newoption {
    trigger = "services-all",
    description = "Include all the services in the engine build"
}

-- Rendering RHI options
newoption {
    trigger = "renderRHI-vulkan",
    description = "Include Vulkan RHI in the engine build"
}

newoption {
    trigger = "vulkan-sdk-path",
    value = "path",
    description = "Path to the Vulkan SDK"
}

filter "options:vulkan-sdk-path"
    IncludeDir["vulkan"] = _OPTIONS["vulkan-sdk-path"] .. "\\Include\\"
    LibraryDir["vulkan"] = _OPTIONS["vulkan-sdk-path"] .. "\\Lib\\"
    BinDir["vulkan"] = _OPTIONS["vulkan-sdk-path"] .. "\\Bin\\"

filter {}

newoption {
    trigger = "renderRHI-opengl",
    description = "Include OpenGL RHI in the engine build"
}

newoption {
    trigger = "renderRHI-dx12",
    description = "Include DirectX12 RHI in the engine build"
}

newoption {
    trigger = "renderRHIs-all",
    description = "Include all rendering RHIs in the engine build"
}

-- Platform options
newoption {
    trigger = "platform-windows",
    description = "Include Windows platform in the engine build"
}

newoption {
    trigger = "platform-mac",
    description = "Include Mac platform in the engine build"
}

newoption {
    trigger = "platform-linux",
    description = "Include Linux platform in the engine build"
}

newoption {
    trigger = "platforms-all",
    description = "Include all platforms in the engine build"
}

-- Deactivate the current filter
filter {}
 
group "Engine"
    include "src\\Coco"

group ""
    include "examples\\CocoSandbox"