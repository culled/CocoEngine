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

    Matrix4x4 projection = RenderService::Get()->GetPlatform().CreateOrthographicProjection(0, viewport.Size.Width, 0, viewport.Size.Height, -100, 100);
    Matrix4x4 view = Matrix4x4::Identity;

    renderView.Setup(viewport, viewport, view, projection, MSAASamples::One, rts);
}
