//
// Created by cullen on 5/14/26.
//

#include "SpritesheetAnimationComponentSystem.h"

#include "Coco/ECS/Rendering/Components/SpriteRendererComponent.h"
#include "Coco/ECS/Rendering/Components/SpritesheetAnimationComponent.h"

namespace Coco
{
    void SpritesheetAnimationComponentSystem::Tick(Scene& scene, const TickInfo& tickInfo)
    {
        auto view = scene.CreateComponentView<SpritesheetAnimationComponent, SpriteRendererComponent>(true);

        for (auto& entity : view)
        {
            auto animComponent = entity.GetComponent<SpritesheetAnimationComponent>();
            auto rendererComponent = entity.GetComponent<SpriteRendererComponent>();

            animComponent->Update(tickInfo);
            rendererComponent->AtlasCellIndex = animComponent->GetCurrentAnimationFrame();
        }
    }
} // Coco