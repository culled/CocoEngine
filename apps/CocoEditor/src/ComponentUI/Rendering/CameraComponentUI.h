#pragma once
#include "../ComponentUI.h"

namespace Coco
{
    class CameraComponentUI :
        public ComponentUI
    {
    protected:
        // Inherited via ComponentUI
        const char* GetHeader() const { return "Camera"; }
        void DrawImpl(ECS::Entity& entity) override;
    };
}