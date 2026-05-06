add_library(Windowing STATIC
        WindowService.h
        WindowService.cpp
        DisplayInfo.h
        WindowingEnginePlatform.h
        Window.cpp
        Window.h
        WindowTypes.h
        WindowTypes.cpp
)

# Force modules we depend on
if(NOT COCO_ENABLE_RENDERING)
    set(COCO_ENABLE_RENDERING ON)
    add_subdirectory(${CMAKE_SOURCE_DIR}/src/Coco/Rendering ${COCO_BUILD_DIR}/Rendering)
endif()

target_link_libraries(Windowing PUBLIC
        Core
        Rendering
)

target_compile_definitions(Windowing PUBLIC
        COCO_SERVICE_WINDOWING
)