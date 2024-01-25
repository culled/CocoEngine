#pragma once
#include <Coco\Rendering\Pipeline\RenderPass.h>
#include <Coco/Rendering/Shader.h>

using namespace Coco;

class TestRenderPass :
    public Rendering::RenderPass
{
public:
    TestRenderPass();

    // Inherited via RenderPass
    const string& GetName() const override { return _name; }
    std::span<const Rendering::RenderPassAttachment> GetAttachments() const override { return _attachments; }
    void Execute(Rendering::RenderContext& context, const Rendering::RenderView& renderView) override;

private:
    static const string _name;
    static const std::array<Rendering::RenderPassAttachment, 1> _attachments;

    SharedRef<Rendering::Shader> _uniformTestShader;
    SharedRef<Rendering::Shader> _textureTestShader;

private:
    void DrawUniformTest(Rendering::RenderContext& context, const Rendering::RenderView& renderView);
    void DrawTextureTest(Rendering::RenderContext& context, const Rendering::RenderView& renderView);
};

