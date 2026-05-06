add_library(ImGui STATIC
        ImGuiService.h
        ImGuiService.cpp
        imgui_coco_config.h
        ImGuiRenderPass.cpp
        ImGuiRenderPass.h
        ImGuiInputLayer.cpp
        ImGuiInputLayer.h
)

target_compile_definitions(ImGui PUBLIC
        COCO_SERVICE_IMGUI
)

FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.92.7-docking
)

FetchContent_MakeAvailable(imgui)

target_include_directories(ImGui PUBLIC
        ${imgui_SOURCE_DIR}
)

add_library(dearimgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
)

target_compile_definitions(dearimgui PUBLIC
        IMGUI_USER_CONFIG="${CMAKE_SOURCE_DIR}/src/Coco/ImGui/imgui_coco_config.h"
)

target_include_directories(dearimgui PRIVATE
        ${imgui_SOURCE_DIR}
)

target_link_libraries(ImGui PUBLIC dearimgui)