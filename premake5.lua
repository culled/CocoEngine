workspace "CocoEngine"
    configurations { "Debug", "Release" }
    platforms { "Win64" }
    architecture "x64"

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
    IncludeDir["uuid_v4"] = "%{wks.location}\\src\\Vendor\\uuid_v4"
    IncludeDir["yaml_cpp"] = "%{wks.location}\\src\\Vendor\\yaml-cpp\\include"

    -- Assets directory
    AssetsDir = "%{wks.location}assets\\"

    -- External program directories
    BinDir = {}

    -- Build options
    newoption {
        trigger = "high-dpi",
        description = "Include high-dpi support in the engine build"
    }

    newoption {
        trigger = "tests",
        description = "Include the test projects"
    }

    -- Service includes
    Services = {}

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
        trigger = "service-imgui",
        description = "Include the ImGui service in the engine build"
    }

    newoption {
        trigger = "service-ecs",
        description = "Include the entity-component-system service in the engine build"
    }

    newoption {
        trigger = "services-all",
        description = "Include all the services in the engine build"
    }

    if (_OPTIONS["service-input"] ~= nil or _OPTIONS["services-all"] ~= nil) then 
        print("Including input service") 
        Services["Input"] = true
    end

    if (_OPTIONS["service-rendering"] ~= nil or _OPTIONS["services-all"] ~= nil) then 
        print("Including rendering service") 
        Services["Rendering"] = true
        IncludeDir["stb"] = "%{wks.location}\\src\\Vendor\\stb"
    end

    if (_OPTIONS["service-windowing"] ~= nil or _OPTIONS["services-all"] ~= nil) then 
        if (Services["Rendering"] ~= true) then
            error("Rendering service must be included with windowing service")
        end

        print("Including windowing service") 
        Services["Windowing"] = true
    end

    if (_OPTIONS["service-imgui"] ~= nil or _OPTIONS["services-all"] ~= nil) then 
        print("Including ImGUI service") 
        Services["ImGui"] = true
        IncludeDir["ImGui"] = "%{wks.location}\\src\\Vendor\\imgui"
        IncludeDir["ImGuizmo"] = "%{wks.location}\\src\\Vendor\\imguizmo"
    end

    if (_OPTIONS["service-ecs"] ~= nil or _OPTIONS["services-all"] ~= nil) then 
        print("Including ECS service") 
        Services["ECS"] = true
        IncludeDir["entt"] = "%{wks.location}\\src\\Vendor\\entt\\src"
    end

    -- Rendering RHI options
    RenderRHI = {}

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

    if (Services["Rendering"] ~= nil) then
        if (_OPTIONS["renderRHI-vulkan"] ~= nil or _OPTIONS["renderRHIs-all"] ~= nil) then
            print("Including Vulkan RHI")
            RenderRHI["Vulkan"] = true

            if (_OPTIONS["vulkan-sdk-path"] == nil) then
                error("Vulkan was included but the Vulkan sdk path was not given")
            else
                print("Vulkan path specified at " .. _OPTIONS["vulkan-sdk-path"])

                IncludeDir["vulkan"] = _OPTIONS["vulkan-sdk-path"] .. "\\Include\\"
                LibraryDir["vulkan"] = _OPTIONS["vulkan-sdk-path"] .. "\\Lib\\"
                BinDir["vulkan"] = _OPTIONS["vulkan-sdk-path"] .. "\\Bin\\"
            end
        end

        if (_OPTIONS["renderRHI-opengl"] ~= nil or _OPTIONS["renderRHIs-all"] ~= nil) then 
            print("Including OpenGL RHI") 
            RenderRHI["OpenGL"] = true
        end

        if (_OPTIONS["renderRHI-dx12"] ~= nil or _OPTIONS["renderRHIs-all"] ~= nil) then 
            print("Including DirectX12 RHI") 
            RenderRHI["DX12"] = true
        end
    end
    
    -- Platform options
    Platforms = {}

    newoption {
        trigger = "platform-win32",
        description = "Include Win32 platform in the engine build"
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

    if (_OPTIONS["platform-win32"] ~= nil or _OPTIONS["platforms-all"] ~= nil) then 
        print("Including Win32 platform")
        Platforms["Win32"] = true
    end

    if (_OPTIONS["platform-osx"] ~= nil or _OPTIONS["platforms-all"] ~= nil) then 
        print("Including Mac platform") 
        Platforms["OSX"] = true
    end

    if (_OPTIONS["platform-linux"] ~= nil or _OPTIONS["platforms-all"] ~= nil) then 
        print("Including Linux platform") 
        Platforms["Linux"] = true
    end

    startproject "CocoEditor"

    if (_ACTION ~= nil) then
        group "Engine"
            include "src\\Coco\\Core"

            if (Services["Input"] == true) then 
                include "src\\Coco\\Input"
            end

            if (Services["Rendering"] == true) then 
                include "src\\Coco\\Rendering"
            end

            if(Services["Windowing"] == true) then         
                include "src\\Coco\\Windowing"
            end

            if(Services["ImGui"] == true) then         
                include "src\\Coco\\ImGUI"
            end

            if(Services["ECS"] == true) then         
                include "src\\Coco\\ECS"
            end

        group "Engine/Platforms"
            if (Platforms["Win32"] == true) then
                include "src\\Coco\\Platforms\\Win32"
            end

        if (_OPTIONS["tests"]) then
            print "Including test projects"
            
            group "Tests"
                include "tests\\Coco\\Core"
        end

        group "Engine/Third Party"
            include "src\\Coco\\Third Party\\yaml-cpp"

        group "Apps"
            include "apps\\CocoEditor"

        group "Example Apps"
            include "exampleapps\\Sandbox"
    end