//
// Created by cullen on 5/10/26.
//

#include "NativeComponentSystem.h"

#include "Coco/ECS/Components/NativeScriptComponent.h"

namespace Coco
{
    void NativeComponentSystem::Tick(Scene& scene, const TickInfo& tickInfo)
    {
        EntityComponentView<NativeScriptComponent> view = scene.CreateComponentView<NativeScriptComponent>(true);

        for (auto& e : view)
        {
            NativeScriptComponent* comp = e.GetComponent<NativeScriptComponent>();
            if (!comp->BoundScript)
                continue;

            comp->BoundScript->Tick(tickInfo);
        }
    }
} // Coco