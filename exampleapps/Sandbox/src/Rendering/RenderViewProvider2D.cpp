#include "RenderViewProvider2D.h"

#include <Coco/Rendering/RenderService.h>

void RenderViewProvider2D::SetupRenderView(
    RenderView& renderView,
    const CompiledRenderPipeline& pipeline, 
    uint64 rendererID, 
    const SizeInt& backbufferSize, 
    std::span<Ref<Image>> backbuffers)
{
    RenderService& rendering = *RenderService::Get();

    std::vector<RenderTarget> rts = rendering.GetAttachmentCache().CreateRenderTargets(pipeline, rendererID, backbufferSize, MSAASamples::One, backbuffers);

    RectInt viewport(Vector2Int::Zero, backbufferSize);

    Matrix4x4 projection = rendering.GetPlatform().CreateOrthographicProjection(0, backbufferSize.Width, 0, backbufferSize.Height, -100, 100);
    Matrix4x4 view = Matrix4x4::Identity;

    renderView.Setup(viewport, viewport, view, projection, Vector3::Zero, ViewFrustum(), MSAASamples::One, rts);
}
