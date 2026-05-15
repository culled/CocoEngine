add_executable(TopDownGame
        src/linuxmain.cpp
        src/GameApplication.cpp
        src/GameApplication.h
        src/WorldManager.h
        src/WorldManager.cpp
        src/NativeScripts/PlayerController.cpp
        src/NativeScripts/PlayerController.h
)

target_link_libraries(TopDownGame PRIVATE
        Core
        Input
        Rendering
        Windowing
        ECS
        ImGui
        LinuxPlatform
)

add_custom_command(TARGET TopDownGame
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Assets ${CMAKE_CURRENT_BINARY_DIR}/Assets
        COMMENT "Copying Assets from '${CMAKE_CURRENT_SOURCE_DIR}/Assets' to binary directory at '${CMAKE_CURRENT_BINARY_DIR}/Assets'")
