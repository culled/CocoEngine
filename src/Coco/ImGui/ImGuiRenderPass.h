//
// Created by cullen on 4/6/26.
//

#ifndef COCOENGINE_IMGUIRENDERPASS_H
#define COCOENGINE_IMGUIRENDERPASS_H
#include "Coco/Core/Math/Matrix4x4.h"
#include "Coco/Core/Math/Rect.h"
#include "Coco/Core/Memory/Ptrs.h"
#include "Coco/Rendering/RenderGraph/RenderGraphTypes.h"

namespace Coco
{
    class RenderGraphBuilder;
    class Texture;
    class Shader;
    class RenderContext;

    /// @brief Data for rendering an ImGui object
    struct ImGuiObjectData
    {
        /// @brief The scissor rectangle to use
        Recti ScissorRect;

        /// @brief The draw texture
        SharedPtr<Texture> DrawTexture;
    };

    /// @brief A render pass for rendering ImGui
    class ImGuiRenderPass
    {
    public:
        ImGuiRenderPass(const Matrix4x4& projection);

        /// @brief Runs when the render pass is created
        /// @param builder The render graph builder
        void Setup(RenderGraphBuilder& builder);

        /// @brief Called when executing the render pass
        /// @param sceneData The scene data
        /// @param ctx The render context
        void Render(const RenderScene& sceneData, RenderContext& ctx) const;

    private:
        static uint64 _shaderResourceID;

        Matrix4x4 _projection;

        /// @brief Ensures the draw shader resource is created
        static void EnsureDrawShader();
    };
} // Coco

#endif //COCOENGINE_IMGUIRENDERPASS_H