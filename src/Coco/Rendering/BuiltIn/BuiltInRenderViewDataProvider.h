#pragma once
#include "../Providers/RenderViewDataProvider.h"
#include <Coco/Core/Types/Color.h>
#include <Coco/Core/Types/Transform.h>
#include "../Graphics/ImageTypes.h"
#include "../Graphics/ShaderUniformLayout.h"

namespace Coco::Rendering
{
    class BuiltInRenderViewDataProvider :
        public RenderViewDataProvider
    {
    public:
        BuiltInRenderViewDataProvider(
            const Color& clearColor = Color::Black,
            const Vector2& depthStencilClearValues = Vector2(1, 0),
            MSAASamples sampleCount = MSAASamples::One,
            std::optional<ShaderUniformLayout> globalUniformLayoutOverride = std::optional<ShaderUniformLayout>());

        // Inherited via RenderViewProvider
        void SetupRenderView(
            RenderView& renderView,
            const CompiledRenderPipeline& pipeline,
            uint64 rendererID,
            const SizeInt& backbufferSize,
            std::span<Ref<Image>> backbuffers) override;

        void SetPerspectiveProjection(double verticalFOVRadians, double nearClip, double farClip);
        void SetOrthographicProjection(double verticalOrthographicSize, double nearClip, double farClip);

        Transform3D& GetViewTransform() { return _viewTransform; }

        void SetClearColor(const Color& clearColor);
        const Color& GetClearColor() const { return _clearColor; }

        void SetDepthStencilClearValues(const Vector2& depthStencilClearValue);
        const Vector2& GetDepthStencilClearValues() const { return _depthStencilClearValues; }

        void SetMSAASampleCount(MSAASamples sampleCount);
        MSAASamples GetMSAASampleCount() const { return _sampleCount; }

    private:
        Color _clearColor;
        Vector2 _depthStencilClearValues;
        MSAASamples _sampleCount;
        std::optional<ShaderUniformLayout> _globalUniformLayoutOverride;

        bool _isPerspective;
        double _projectionSize;
        double _nearClip;
        double _farClip;

        Transform3D _viewTransform;
    };
}