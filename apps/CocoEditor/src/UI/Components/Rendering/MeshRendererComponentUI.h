#pragma once
#include "../../ComponentUI.h"

namespace Coco
{
    class MeshRendererComponentUI :
        public ComponentUI
    {
    protected:
        // Inherited via ComponentUI
        const char* GetHeader() const override { return "Mesh Renderer"; }
        void DrawPropertiesImpl(ECS::Entity& entity) override;
        void DrawGizmosImpl(ECS::Entity& entity, const SizeInt& viewportSize) override;
    };
}