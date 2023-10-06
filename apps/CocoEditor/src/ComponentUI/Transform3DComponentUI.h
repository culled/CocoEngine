#pragma once
#include "ComponentUI.h"

namespace Coco
{
    class Transform3DComponentUI :
        public ComponentUI
    {
        // Inherited via ComponentUI
        void DrawComponent(ECS::Entity& entity) override;
    };
}