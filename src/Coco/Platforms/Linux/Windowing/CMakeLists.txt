target_sources(LinuxPlatform PRIVATE
        LinuxWindowSystem.h
        LinuxWindowSystem.cpp
        X11/X11Includes.h
        X11/X11Types.h
        X11/X11Window.h
        X11/X11Window.cpp
        X11/X11WindowSystem.h
        X11/X11WindowSystem.cpp
)

target_link_libraries(LinuxPlatform PUBLIC
        Windowing
)

if(COCO_RENDERING_INCLUDE_OPENGL)
    add_subdirectory(X11/OpenGL)
endif ()

if(COCO_RENDERING_INCLUDE_VULKAN)
    add_subdirectory(X11/Vulkan)
endif ()

