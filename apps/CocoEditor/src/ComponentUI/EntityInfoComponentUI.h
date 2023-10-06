#pragma once
#include "ComponentUI.h"

namespace Coco
{
    class EntityInfoComponentUI :
        public ComponentUI
    {
        // Inherited via ComponentUI
        void DrawComponent(ECS::Entity& entity) override;
    };
}