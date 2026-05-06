add_executable(Sandbox
        src/linuxmain.cpp
        src/SandboxApplication.cpp
        src/SandboxApplication.h
)

target_link_libraries(Sandbox PRIVATE
        Core
        Input
        Rendering
        Windowing
        ECS
        ImGui
        LinuxPlatform
)

add_custom_command(TARGET Sandbox
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Assets ${CMAKE_CURRENT_BINARY_DIR}/Assets
        COMMENT "Copying Assets from '${CMAKE_SOURCE_DIR}/Assets' to binary directory at '${CMAKE_CURRENT_BINARY_DIR}/Assets'")
