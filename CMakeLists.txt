cmake_minimum_required(VERSION 4.1)
include(ExternalProject)
include(FetchContent)

project(CocoEngine)

option(COCO_ENABLE_INPUT "Adds support for input from external devices" ON)
option(COCO_ENABLE_RENDERING "Adds support for rendering" ON)
option(COCO_ENABLE_WINDOWING "Adds support for creating GUI windows (requires the rendering service)" ON)
option(COCO_ENABLE_ECS "Adds support for scenes using the Entity-Component-System" ON)
option(COCO_ENABLE_IMGUI "Adds support for using ImGui" ON)
option(COCO_ENABLE_PHYSICS "Adds support for using physics" ON)
option(COCO_ENABLE_APPS "Adds example apps" ON)

set(CMAKE_CXX_STANDARD 20)              # Set the C++20 standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)     # Enforce the standard
set(CMAKE_CXX_EXTENSIONS OFF)           # Disable non-standard extensions (e.g., GNU++17)


add_compile_options(
        #-fsanitize=address,undefined
        #-fno-omit-frame-pointer
        #-Wpedantic
        #-Wextra
        #-Wall
        #-Werror=return-type
)

add_link_options(
        #-fsanitize=address,undefined
)

add_compile_definitions(COCO_ASSERTIONS)
include_directories(src)

set(COCO_BUILD_DIR ${CMAKE_BINARY_DIR}/build/Coco)

# Core includes
add_subdirectory(src/Coco/Core ${COCO_BUILD_DIR}/Core)

# Include services
if(COCO_ENABLE_INPUT)
    add_subdirectory(src/Coco/Input ${COCO_BUILD_DIR}/Input)
endif()

if(COCO_ENABLE_RENDERING)
    add_subdirectory(src/Coco/Rendering ${COCO_BUILD_DIR}/Rendering)
endif()

if(COCO_ENABLE_WINDOWING)
    add_subdirectory(src/Coco/Windowing ${COCO_BUILD_DIR}/Windowing)
endif()

if(COCO_ENABLE_ECS)
    add_subdirectory(src/Coco/ECS ${COCO_BUILD_DIR}/ECS)
endif()

if(COCO_ENABLE_IMGUI)
    add_subdirectory(src/Coco/ImGui ${COCO_BUILD_DIR}/ImGui)
endif()

if(COCO_ENABLE_PHYSICS)
    add_subdirectory(src/Coco/Physics ${COCO_BUILD_DIR}/Physics)
endif()

# Include platform
if(UNIX)
    add_subdirectory(src/Coco/Platforms/Linux ${COCO_BUILD_DIR}/Platforms/Linux)
endif()

# Include apps
if(COCO_ENABLE_APPS)
    set(COCO_APPS_BUILD_DIR ${CMAKE_BINARY_DIR}/build/Apps)

    add_subdirectory(Apps/Sandbox ${COCO_APPS_BUILD_DIR}/Sandbox)

    #add_subdirectory(Apps/TopDownGame ${COCO_APPS_BUILD_DIR}/TopDownGame)
endif()