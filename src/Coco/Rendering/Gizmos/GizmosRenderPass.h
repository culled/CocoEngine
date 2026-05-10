//
// Created by cullen on 5/9/26.
//

#ifndef COCOENGINE_GIZMOSRENDERPASS_H
#define COCOENGINE_GIZMOSRENDERPASS_H
#include "Coco/Core/Math/Matrix4x4.h"

namespace Coco
{
    class RenderContext;
    class RenderScene;
    class RenderGraphBuilder;

    /// @brief Draw data for a gizmo
    struct GizmoObjectData
    {
        /// @brief The gizmo transform
        Matrix4x4 Transform;

        /// @brief The gizmo color
        Vector4 Color;
    };

    /// @brief A render pass for rendering gizmos
    class GizmosRenderPass
    {
    public:
        GizmosRenderPass();

        /// @brief Runs when the render pass is created
        /// @param builder The render graph builder
        void Setup(RenderGraphBuilder& builder);

        /// @brief Called when executing the render pass
        /// @param sceneData The scene data
        /// @param ctx The render context
        void Render(const RenderScene& sceneData, RenderContext& ctx) const;

    private:
        static uint64 _shaderResourceID;

        /// @brief Ensures the draw shader resource is created
        static void EnsureDrawShader();
    };
} // Coco

#endif //COCOENGINE_GIZMOSRENDERPASS_H