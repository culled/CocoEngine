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