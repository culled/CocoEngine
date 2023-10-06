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
        void DrawImpl(ECS::Entity& entity) override;
    };
}