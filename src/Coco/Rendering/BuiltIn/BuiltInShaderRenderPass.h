#pragma once
#include "../Pipeline/RenderPass.h"
#include "../Shader.h"

namespace Coco::Rendering
{
    class BuiltInShaderRenderPass :
        public RenderPass
    {
    public:
        /// @brief The visibility group for objects to render with the lit shader
        static const uint64 LitVisibilityGroup;

        /// @brief The visibility group for objects to render with the unlit shader
        static const uint64 UnlitVisibilityGroup;

    public:
        BuiltInShaderRenderPass(bool useFrustumCulling);

        // Inherited via RenderPass
        const string& GetName() const override { return _name; }
        std::span<const RenderPassAttachment> GetAttachments() const override { return _attachments; }
        void Execute(RenderContext& context, const RenderView& renderView) override;

        /// @brief Sets this render pass to use frustum culling
        /// @param useFrustumCulling If true, this pass will use frustum culling
        void SetUseFrustumCulling(bool useFrustumCulling);

        /// @brief Gets if this render pass is using frustum culling
        /// @return True if this pass is using frustum culling
        bool IsUsingFrustumCulling() const { return _frustumCulling; }

    private:
        static const std::array<RenderPassAttachment, 2> _attachments;
        static const string _name;

        SharedRef<Shader> _litShader;
        SharedRef<Shader> _unlitShader;
        bool _frustumCulling;

    private:
        /// @brief Renders all objects that use the "Unlit" shader variant
        /// @param context The render context
        /// @param renderView The render view
        /// @param objectIndices The objects that should be considered to render
        void RenderUnlitObjects(RenderContext& context, const RenderView& renderView, const std::vector<uint64>& objectIndices);

        /// @brief Renders all objects that use the "Lit" shader variant
        /// @param context The render context
        /// @param renderView The render view
        /// @param objectIndices The objects that should be considered to render
        void RenderLitObjects(RenderContext& context, const RenderView& renderView, const std::vector<uint64>& objectIndices);

        /// @brief Renders all the objects using a given shader variant
        /// @param context The render context
        /// @param renderView The render view
        /// @param objectIndices The objects that should be rendered
        /// @param shader The shader to render with
        void RenderObjects(RenderContext& context, const RenderView& renderView, const std::vector<uint64>& objectIndices, const SharedRef<Shader>& shader);
    };
}