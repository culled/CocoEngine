#include "BasicRenderViewProvider.h"

#include <Coco/Rendering/RenderService.h>

BasicRenderViewProvider::BasicRenderViewProvider() :
    _clearColor(Color(0.1, 0.2, 0.3, 1.0))
{}

UniqueRef<RenderView> BasicRenderViewProvider::CreateRenderView(const RenderPipeline& pipeline, const SizeInt& backbufferSize, std::span<Ref<Image>> backbuffers)
{
    std::vector<RenderTarget> rts;

    for (size_t i = 0; i < backbuffers.size(); i++)
    {
        rts.emplace_back(backbuffers[i], _clearColor);
    }

    double aspectRatio = static_cast<double>(backbufferSize.Width) / backbufferSize.Height;
    RectInt viewport(Vector2Int::Zero, backbufferSize);
    Matrix4x4 view = Matrix4x4::CreateLookAtMatrix(Vector3::One, Vector3::Zero, Vector3::Up);
    //Matrix4x4 view = Matrix4x4::CreateLookAtMatrix(Vector3::Backward, Vector3::Forward, Vector3::Up);
    //Matrix4x4 projection = RenderService::Get()->GetPlatform()->CreateOrthographicProjection(2.0, aspectRatio, 0.1, 100);
    Matrix4x4 projection = RenderService::Get()->GetPlatform()->CreatePerspectiveProjection(Math::DegToRad(90.0), aspectRatio, 0.1, 100);

    return CreateUniqueRef<RenderView>(viewport, viewport, view, projection, rts);
}
