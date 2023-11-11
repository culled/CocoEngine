#pragma once
#include "../../ComponentUI.h"

namespace Coco::Rendering
{
    class Material;
    enum class ShaderUniformType;
}

namespace Coco
{
    class MeshRendererComponentUI :
        public ComponentUI
    {
    private:
        std::array<string, 64> _flagTexts;
        std::array<string, 12> _uniformTypeTexts;
        Rendering::ShaderUniformType _addParamUniformType;
        string _addParamUniformName;
        bool _isAddingParam;
        string _removeParamUniformName;
        bool _isRemovingParam;

    public:
        MeshRendererComponentUI();

    protected:
        // Inherited via ComponentUI
        const char* GetHeader() const override { return "Mesh Renderer"; }
        void DrawPropertiesImpl(ECS::Entity& entity) override;
        void DrawGizmosImpl(ECS::Entity& entity, const SizeInt& viewportSize) override;

    private:
        string GetVisibilityFlagText(uint64 flags, int maxFlags = 2);

        void DrawAddParameterSection(Rendering::Material& material);
        void DrawRemoveParameterSection(Rendering::Material& material);
    };
}