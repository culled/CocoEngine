add_library(Input STATIC
        InputService.h
        InputService.cpp
        MouseTypes.h
        InputEvent.cpp
        InputEvent.h
        Mouse.cpp
        Mouse.h
        MouseTypes.cpp
        InputLayer.cpp
        InputLayer.h
        Keyboard.cpp
        Keyboard.h
        KeyboardTypes.h
        KeyboardTypes.cpp
        InputEnginePlatform.h
)

target_link_libraries(Input PUBLIC
        Core
)

target_compile_definitions(Input PUBLIC
        COCO_SERVICE_INPUT
)