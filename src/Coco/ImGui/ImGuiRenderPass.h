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
        static const string Name;
        static const uint64 VisibilityGroup;

    public:
        ImGuiRenderPass();

        // Inherited via RenderPass
        const string& GetName() const final { return Name; }
        std::span<const RenderPassAttachment> GetAttachments() const final { return _attachments; }
        void Execute(RenderContext& context, const RenderView& renderView) override;

    private:
        static const string _shaderName;
        static const std::array<RenderPassAttachment, 1> _attachments;
        static SharedRef<Shader> _shader;

    private:
        /// @brief Creates the ImGui shader
        static void CreateShader();
    };
}