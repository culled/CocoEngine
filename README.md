# CocoEngine
A modular, flexible C++ game engine built to run on multiple platforms.

The engine is composed of a core that uses `EngineServices` to implement functionality, such as rendering, physics, an entity-component system, and more, as well as an `Application` class that client applications derive from. The core creates an instance of the application and runs a process loop that dispatches ticks to listeners, and can be run headless without any other services.

The engine is designed to be relatively unopinionated in its functionality, which means that it is very flexible but it also requires you to define your own structure for developing games and applications using it. Right now, it is more akin to a framework as opposed to a full-fledged editor and suite like Godot, Unreal Engine, or Unity.

This project is a hobby of mine and the engine is still under heavy development. Many features are being implemented as I need them and as I learn more. The engine should be considered as a **"minimum viable product"** right now. It definitely needs a lot of work in terms of optimization and resiliency, and I will address that as I find the time and energy to. Comments and documentation will also be added once I feel like certain APIs have stabilized. I'm mostly doing this as a learning experience because I love building systems that work together, and this project has taught me so much.

If you want to use this engine for your own projects, by all means have fun with it!

## Core Features
#### File IO
Files can be written to or read utilizing a `FileSystem.` Using a file system allows for the engine to find files in a variety of folders during development, then read the same files in a compressed package for production releases without modifying user code.

#### Logging
Logs can be written to a number of `LogSinks` that can, for example, write log messages both to a console output and to a log file. Multiple message severities are supported, which can be used to, for example, filter out verbose and informational messages from error messages.

#### Math
For API consistency, various math functions have been exposed, as well as common constant values and other useful functions. Various mathematical objects commonly used in games also exist, such as vectors, quaternions, matrices, and more.

#### Memory Manager and Allocation Strategies
A memory manager tracks the usage of different parts of the engine. All dynamic memory is allocated via custom allocators that can be used for different types of memory allocation requirements. Smart memory managing objects are also provided to simplify memory management and prevent memory leaks.

#### Resource Management
A `ResourceManager` is used to hold and access shared resources, such as meshes, textures, and more within the engine. Resources can also be saved and loaded from disk.

#### Static Runtime Typing
Systems can opt-in to static runtime typing scheme that enables typing and polymorphic information to be derived about them. Through a few simple macro lines, types are automatically registered to the system at startup.

#### Event System
An event system is provided that allows subscribers to listen to events and execute functions when those events are invoked. Events can also "handled", which prevents them from dispatching to other listeners.

# EngineServices
## Input
Provides basic input functionality. Input events are gathered from the platform, state for input devices is updated, and then input events are dispatched to input layers that can allow the propagation of events. This allows, for example, a "UI" input layer to consume input events where applicable and prevent them from being passed to subsequent layers. The state of input devices can also be polled each frame. Raw mouse input is supported on platforms that support it.

#### Supported Input Devices
- Mouse
- Keyboard
- Touchscreen (planned)
- Gamepad (planned)

## Rendering
Provides rendering functionality to draw to images or GUI windows. `FinalRenderTargets` are added to the rendering system, which then calls registered `RenderListeners` that dispatch callbacks to provide rendering instructions to draw on them. `RenderListeners` are organized by a defined order, which allows for a layer-like approach to adding `RenderPasses` that define the actual rendering operations. The renderer builds a basic graph that links resources used by `RenderPasses` and culls any passes that don't contribute to the final image. All resources for a render are stored in a `RenderScene` to allow for future asynchronous rendering, allowing for the game world to change without interfering with the rendering of past frames.

#### Supported Render APIs
- Vulkan
- DirectX 12 (planned)
- Metal (planned)

## Windowing
Provides GUI windows that can be drawn to using the Rendering service. On platforms that support it, multiple windows are supported. The first window created becomes the "main window" that closes the entire application when it is closed. All windows that are shown automatically become `FinalRenderTargets` for the rendering system.

#### Supported Window APIs
##### Linux
- X11
- Wayland (planned)

##### Windows (planned)

##### OSX (planned)

## ImGui
Allows for using ImGui to render panels and other UI elements using the Rendering service. The windows are drawn in their own `RenderPass` that is automatically added to the render pipeline. Docking support is planned, and having ImGui windows detach from the main window is also planned.

## ECS (Entity Component System)
Provides support for structuring scenes with entities that can have components attached to them to give them meaning and functionality. The ECS system utilizes the static RTTI system, which should allow for full polymorphic components.

Components for other EngineServices are included if those services are compiled with the engine.

## Physics (planned)
Provides physics that allows objects to react to gravity and collision forces.

#### Supported Physics APIs
- Jolt (planned)

# Supported Platforms
- Linux
- Windows (planned)
- OSX (planned)

# Creating an Application and Running It
In order to create an application using the engine, you must create a C++ project and statically link the engine to it. Then, you must make a class that derives from the `Application` class. An instance of this class will automatically be created when the engine starts. You must also create an entrypoint script that sets up the engine's platform and creates the engine. An example entrypoint can be found in `main.cpp` of the "Sandbox" project in the "Apps" directory.

# Project Setup and Building
The engine uses CMake 4.1+ as the build system, which should automatically fetch dependencies upon project setup.

#### Manual build steps:
1. Open the "CocoEngine" root folder in a terminal
2. Configure the project: ```cmake .```
3. Build the project: ```cmake --build .```

Right now, the entire project is statically built and linked. You can also include/exclude the compiling of different services by modifying the CMake options. By default, the project includes a "Sandbox" application that can be used to experiment with and develop the engine. If you want to create your own application, you can create your own C++ project and include it as a CMake target in the root `CMakeLists.txt` file.

You will need to either copy the "Assets" directory to the same directory as your compiled executable file, or set the working directory of your project to be the root of the project. This is because the engine currently searches for the "Assets" directory relative to the directory of the current process for loading shaders and other assets it uses.

I'm still relatively new to CMake, and I'm sure the build process needs to be updated to be more resilient and flexible, but it works for what I need right now. I currently use JetBrains CLion as my primary IDE, which should automatically set up the project correctly once you open it.

# Project Structure
All source code for the engine can be found in the "src/Coco" directory. Each service and platform are organized into their own folder. Any third-party scripts integrated into the engine's build system are located in various "Vendor" directories within each project. A top-level "Vendor" directory in the "src" directory can also be found, which is used for including projects that can't be included via CMake (such as glad).

Example applications for the engine can be found in the "Apps" directory.

## Third Party Dependencies
- [Slang](https://github.com/shader-slang/slang)
- [VulkanMemoryAllocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator)
- [Vulkan](https://www.vulkan.org/)
- [glad](https://glad.dav1d.de/)
- [stb](https://github.com/nothings/stb)
- [ImGui](https://github.com/ocornut/imgui)

