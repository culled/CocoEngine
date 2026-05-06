target_sources(ECS PRIVATE
        Components/CameraComponent.h
        Components/CameraComponent.cpp
        Components/SpriteRendererComponent.h
        Components/SpriteRendererComponent.cpp
        Components/MeshRendererComponent.h
        Components/MeshRendererComponent.cpp
        Components/SpritesheetAnimationComponent.h
        Components/SpritesheetAnimationComponent.cpp
        Components/TileMapRendererComponent.h
        Components/TileMapRendererComponent.cpp
)

target_link_libraries(ECS PUBLIC
        Rendering
)