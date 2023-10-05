#pragma once
#include "../RenderPass.h"

namespace Coco::Rendering
{
    /// @brief A render pass for rendering objects using the built-in shaders
    class BuiltInRenderPass :
        public RenderPass
    {
    public:
        /// @brief The group tag of shaders that this pass will consider rendering
        static const string sGroupTag;

    private:
        static const std::array<AttachmentFormat, 2> _sAttachments;

        bool _frustumCulling;

    public:
        BuiltInRenderPass(bool useFrustumCulling);

        // Inherited via RenderPass
        const char* GetName() const final { return "Built-In"; }
        std::span<const AttachmentFormat> GetInputAttachments() const final { return _sAttachments; }
        bool SupportsMSAA() const final { return true; }
        void Prepare(RenderContext& context, const RenderView& renderView) final;
        void Execute(RenderContext& context, const RenderView& renderView) final;

        /// @brief Sets this render pass to use frustum culling
        /// @param useFrustumCulling If true, this pass will use frustum culling
        void SetUseFrustumCulling(bool useFrustumCulling);

        /// @brief Gets if this render pass is using frustum culling
        /// @return True if this pass is using frustum culling
        bool IsUsingFrustumCulling() const { return _frustumCulling; }

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
        /// @param shaderVariantName The name of the shader variant to render with
        void RenderObjects(RenderContext& context, const RenderView& renderView, const std::vector<uint64>& objectIndices, const string& shaderVariantName);
    };
}