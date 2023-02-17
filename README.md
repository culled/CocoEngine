# CocoEngine

A work in progress...

## Supported Platforms
* Windows x64

## Building
1. Clone this repository to your machine: `git clone https://github.com/culled/CocoEngine.git`
2. Enter the project directory: `cd CocoEngine`
3. Clone dependent submodules: `git submodule update --init --recursive`
4. Run the configure script for your environment located in the "scripts" directory. This will generate a Visual Studio solution and projects: `cd scripts`
  1. For Windows using Visual Studio 2022, run the "configure-win-x64-vs2022.bat" script: `./configure-win-x64-vs2022.bat`
5. Open the Visual Studio solution and build like any other project!