target_sources(LinuxPlatform PRIVATE
        X11VulkanGraphicsSurfaceFactory.h
        X11VulkanGraphicsSurfaceFactory.cpp
)

target_include_directories(LinuxPlatform PRIVATE
        ${volk_SOURCE_DIR}
)

target_link_libraries(LinuxPlatform PRIVATE
        volk
)