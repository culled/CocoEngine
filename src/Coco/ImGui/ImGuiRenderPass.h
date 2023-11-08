#pragma once
#include <Coco/Rendering/Pipeline/RenderPass.h>
#include <Coco/Rendering/Shader.h>

#include <vector>

namespace Coco::ImGuiCoco
{
    using namespace Coco::Rendering;

    /// @brief A RenderPass for ImGui
    class ImGuiRenderPass :
        public RenderPass
    {
        friend class ImGuiCocoPlatform;

    public:
        static const string sPassName;

    private:
        static const uint64 _sVisibilityGroup;
        static const string _sShaderName;
        static SharedRef<Shader> _sShader;

        std::vector<AttachmentFormat> _attachments;

    public:
        ImGuiRenderPass();

        // Inherited via RenderPass
        const char* GetName() const final { return sPassName.c_str(); }
        std::span<const AttachmentFormat> GetInputAttachments() const final { return _attachments; }
        void Prepare(RenderContext& context, const RenderView& renderView) override;
        void Execute(RenderContext& context, const RenderView& renderView) override;

    private:
        /// @brief Creates the ImGui shader
        static void CreateShader();
    };
}