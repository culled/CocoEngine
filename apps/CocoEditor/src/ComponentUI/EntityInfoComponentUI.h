#pragma once
#include "ComponentUI.h"

namespace Coco
{
    class EntityInfoComponentUI :
        public ComponentUI
    {
    protected:
        // Inherited via ComponentUI
        const char* GetHeader() const override { return "Entity Info"; }
        void DrawImpl(ECS::Entity& entity) override;
    };
}