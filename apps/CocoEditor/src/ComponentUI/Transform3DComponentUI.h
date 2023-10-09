#pragma once
#include "ComponentUI.h"

namespace Coco
{
    class Transform3DComponentUI :
        public ComponentUI
    {
    protected:
        // Inherited via ComponentUI
        const char* GetHeader() const override { return "Transform 3D"; }
        void DrawPropertiesImpl(ECS::Entity& entity) override;
        void DrawGizmosImpl(ECS::Entity& entity, const SizeInt& viewportSize) override {}
    };
}