#pragma once
#include <Coco\Rendering\Pipeline\RenderPass.h>
#include <Coco/Core/Types/Vector.h>
#include <Coco/Rendering/Shader.h>

namespace Coco
{
    class PickingRenderPass :
        public Rendering::RenderPass
    {
    public:
        static const string sName;
        
    private:
        static std::vector<Rendering::AttachmentFormat> _sAttachments;
        static SharedRef<Rendering::Shader> _sShader;

    public:
        PickingRenderPass();

        // Inherited via RenderPass
        const char* GetName() const override { return sName.c_str(); }
        std::span<const Rendering::AttachmentFormat> GetInputAttachments() const override { return _sAttachments; }
        void Prepare(Rendering::RenderContext& context, const Rendering::RenderView& renderView) override;
        void Execute(Rendering::RenderContext& context, const Rendering::RenderView& renderView) override;

    private:
        static void CreateShader();
    };
}