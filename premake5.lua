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

    if (_OPTIONS["service-input"] ~= nil or _OPTIONS["services-all"] ~= nil) then print("Including input service") end
    if (_OPTIONS["service-rendering"] ~= nil or _OPTIONS["services-all"] ~= nil) then print("Including rendering service") end
    if (_OPTIONS["service-windowing"] ~= nil or _OPTIONS["service-rendering"] ~= nil or _OPTIONS["services-all"] ~= nil) then print("Including windowing service") end

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

    if (_OPTIONS["renderRHI-vulkan"] ~= nil or _OPTIONS["renderRHIs-all"] ~= nil) then print("Including Vulkan RHI") end
    if (_OPTIONS["renderRHI-opengl"] ~= nil or _OPTIONS["renderRHIs-all"] ~= nil) then print("Including OpenGL RHI") end
    if (_OPTIONS["renderRHI-dx12"] ~= nil or _OPTIONS["renderRHIs-all"] ~= nil) then print("Including DirectX12 RHI") end

    if (_OPTIONS["renderRHI-vulkan"] ~= nil or _OPTIONS["renderRHIs-all"] ~= nil) then
        if (_OPTIONS["vulkan-sdk-path"] == nil) then
            error("Vulkan was included but the Vulkan sdk path was not given")
        else
            print("Vulkan path specified at " .. _OPTIONS["vulkan-sdk-path"])

            IncludeDir["vulkan"] = _OPTIONS["vulkan-sdk-path"] .. "\\Include\\"
            LibraryDir["vulkan"] = _OPTIONS["vulkan-sdk-path"] .. "\\Lib\\"
            BinDir["vulkan"] = _OPTIONS["vulkan-sdk-path"] .. "\\Bin\\"
        end
    end
    
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

    if (_OPTIONS["platform-windows"] ~= nil or _OPTIONS["platforms-all"] ~= nil) then print("Including Windows platform") end
    if (_OPTIONS["platform-mac"] ~= nil or _OPTIONS["platforms-all"] ~= nil) then print("Including Mac platform") end
    if (_OPTIONS["platform-linux"] ~= nil or _OPTIONS["platforms-all"] ~= nil) then print("Including Linux platform") end
    
    if (_ACTION ~= nil) then
        group "Engine"
            include "src\\Coco"

        group ""
            include "examples\\CocoSandbox"
            include "examples\\Breakout"
    end