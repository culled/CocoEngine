#pragma once
#include "../../ComponentUI.h"

namespace Coco
{
    class CameraComponentUI :
        public ComponentUI
    {
    protected:
        // Inherited via ComponentUI
        const char* GetHeader() const { return "Camera"; }
        void DrawPropertiesImpl(ECS::Entity& entity) override;
        void DrawGizmosImpl(ECS::Entity& entity, const SizeInt& viewportSize) override;
    };
}