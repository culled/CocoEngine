add_library(LinuxPlatform STATIC
        LinuxEnginePlatform.cpp
        LinuxEnginePlatform.h
)

target_link_libraries(LinuxPlatform PUBLIC
        Core
)

target_compile_definitions(LinuxPlatform PUBLIC
        COCO_PLATFORM_LINUX
)

find_package(X11 REQUIRED)

target_include_directories(LinuxPlatform PRIVATE
        ${X11_INCLUDE_DIRS}
        ${X11_Xrandr_INCLUDE_PATH}
        ${X11_Xi_INCLUDE_PATH}
)

target_link_libraries(LinuxPlatform PRIVATE
        ${X11_LIBRARIES}
        ${X11_Xrandr_LIB}
        ${X11_Xi_LIB}
)

if(COCO_ENABLE_INPUT)
    target_link_libraries(LinuxPlatform PUBLIC
            Input
    )
endif ()

if(COCO_ENABLE_RENDERING)
    target_link_libraries(LinuxPlatform PUBLIC
            Rendering
    )
endif ()

if(COCO_ENABLE_WINDOWING)
    add_subdirectory(Windowing)
endif ()