#pragma once
#include "../ComponentUI.h"

namespace Coco
{
    class EntityInfoComponentUI :
        public ComponentUI
    {
    protected:
        // Inherited via ComponentUI
        const char* GetHeader() const override { return "Entity Info"; }
        void DrawPropertiesImpl(ECS::Entity& entity) override;
        void DrawGizmosImpl(ECS::Entity& entity, const SizeInt& viewportSize) override {}
    };
}