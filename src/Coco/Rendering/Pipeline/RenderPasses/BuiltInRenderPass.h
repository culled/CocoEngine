#pragma once
#include "../RenderPass.h"
#include "../../Shader.h"

namespace Coco::Rendering
{
    /// @brief A render pass for rendering objects using the built-in shaders
    class BuiltInRenderPass :
        public RenderPass
    {
    public:
        /// @brief The visibility group for objects to render with the lit shader
        static const uint64 sLitVisibilityGroup;

        /// @brief The visibility group for objects to render with the unlit shader
        static const uint64 sUnlitVisibilityGroup;

    private:
        static const std::array<AttachmentFormat, 2> _sAttachments;
        static SharedRef<Shader> _sLitShader;
        static SharedRef<Shader> _sUnlitShader;

        bool _frustumCulling;

    public:
        BuiltInRenderPass(bool useFrustumCulling);

        // Inherited via RenderPass
        const char* GetName() const final { return "Built-In"; }
        std::span<const AttachmentFormat> GetInputAttachments() const final { return _sAttachments; }
        void Prepare(RenderContext& context, const RenderView& renderView) final;
        void Execute(RenderContext& context, const RenderView& renderView) final;

        /// @brief Sets this render pass to use frustum culling
        /// @param useFrustumCulling If true, this pass will use frustum culling
        void SetUseFrustumCulling(bool useFrustumCulling);

        /// @brief Gets if this render pass is using frustum culling
        /// @return True if this pass is using frustum culling
        bool IsUsingFrustumCulling() const { return _frustumCulling; }

    private:
        /// @brief Creates the built-in shaders
        static void CreateShaders();

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
        /// @param shaderName The name of the shader to render with
        void RenderObjects(RenderContext& context, const RenderView& renderView, const std::vector<uint64>& objectIndices, const string& shaderName);
    };
}