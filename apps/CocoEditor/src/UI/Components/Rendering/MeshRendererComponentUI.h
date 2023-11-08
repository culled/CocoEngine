#pragma once
#include "../../ComponentUI.h"

namespace Coco
{
    class MeshRendererComponentUI :
        public ComponentUI
    {
    private:
        std::array<string, 64> _flagTexts;

    public:
        MeshRendererComponentUI();

    protected:
        // Inherited via ComponentUI
        const char* GetHeader() const override { return "Mesh Renderer"; }
        void DrawPropertiesImpl(ECS::Entity& entity) override;
        void DrawGizmosImpl(ECS::Entity& entity, const SizeInt& viewportSize) override;

    private:
        string GetVisibilityFlagText(uint64 flags, int maxFlags = 2);
    };
}