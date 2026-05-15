add_library(ECS STATIC
    ECSService.h
    ECSService.cpp
        Entity.cpp
        Entity.h
        EntityStorage.cpp
        EntityStorage.h
        Scene.cpp
        Scene.h
        EntityComponent.cpp
        EntityComponent.h
        EntityComponentStorage.cpp
        EntityComponentStorage.h
        EntityComponentRegistry.h
        Components/Transform3DComponent.cpp
        Components/Transform3DComponent.h
        EntityChildView.cpp
        EntityChildView.h
        EntityComponentView.h
        Components/Transform2DComponent.cpp
        Components/Transform2DComponent.h
        Components/NativeScriptComponent.cpp
        Components/NativeScriptComponent.h
        Systems/NativeComponentSystem.cpp
        Systems/NativeComponentSystem.h
)

target_link_libraries(ECS PUBLIC
        Core
)

target_compile_definitions(ECS PUBLIC
        COCO_SERVICE_ECS
)

if(COCO_ENABLE_RENDERING)
    add_subdirectory(Rendering)
endif()