#include "RenderViewProvider2D.h"

#include <Coco/Rendering/RenderService.h>

RenderViewProvider2D::RenderViewProvider2D(AttachmentCache& attachmentCache) :
    _attachmentCache(attachmentCache)
{}

void RenderViewProvider2D::SetupRenderView(
    RenderView& renderView,
    const CompiledRenderPipeline& pipeline, 
    uint64 rendererID, 
    const SizeInt& backbufferSize, 
    std::span<Ref<Image>> backbuffers)
{
    std::vector<RenderTarget> rts = _attachmentCache.CreateRenderTargets(pipeline, rendererID, backbufferSize, MSAASamples::One, backbuffers);

    RectInt viewport(Vector2Int::Zero, backbufferSize);

    Matrix4x4 projection = RenderService::Get()->GetPlatform().CreateOrthographicProjection(0, backbufferSize.Width, 0, backbufferSize.Height, -100, 100);
    Matrix4x4 view = Matrix4x4::Identity;

    renderView.Setup(viewport, viewport, view, projection, Vector3::Zero, ViewFrustum(), MSAASamples::One, rts);

    GlobalShaderUniformLayout globalLayout(
        {
            ShaderDataUniform("ProjectionMatrix", ShaderStageFlags::Vertex, BufferDataType::Mat4x4),
            ShaderDataUniform("ViewMatrix", ShaderStageFlags::Vertex, BufferDataType::Mat4x4)
        },
        {},
        {}
    );

    renderView.SetGlobalUniformLayout(globalLayout);
}
