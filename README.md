# CocoEngine

A modular C++ realtime game engine.

## Services
* Windowing
  - Allows creating GUI windows on a platform and drawing rendered output
* Input
  - Receives input from the platform and keeps track of mouse and keyboard states (more input types planned)
* Rendering
  - Allows rendering images
  - Render RHIs supported:
    + Vulkan
* ImGui
  - Allows using ImGui with the engine

## Supported Platforms
* Windows x64 (Win32)

## Building
1. Download version 1.3.268.0 of the VulkanSDK from [LunarXchange](https://vulkan.lunarg.com/) - ensure you include the 64-bit shader tools in the installation for shaderc and spirv-cross (this will be addressed eventually)
2. Clone this repository to your machine: `git clone https://github.com/culled/CocoEngine.git`
3. Enter the project directory: `cd CocoEngine`
4. Clone dependent submodules: `git submodule update --init --recursive`
5. Run the configure script for your environment located in the "scripts" directory. This will generate a Visual Studio solution and projects: `cd scripts`
  - For Windows using Visual Studio 2022, run the "configure-win-x64-vs2022.bat" script: `./configure-win-x64-vs2022.bat`
6. Open the Visual Studio solution and build like any other project!
