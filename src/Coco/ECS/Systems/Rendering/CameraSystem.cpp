#include "ECSpch.h"
#include "CameraSystem.h"

#include "../../Components/Rendering/CameraComponent.h"
#include "../../Components/Transform3DComponent.h"
#include "SceneRenderProvider.h"

#include <Coco/Rendering/RenderService.h>

namespace Coco::ECS
{
	CameraRenderViewProvider::CameraRenderViewProvider(const Entity& camera) :
		_entity(camera)
	{}

	void CameraRenderViewProvider::SetupRenderView(
		RenderView& renderView, 
		const CompiledRenderPipeline& pipeline, 
		uint64 rendererID, 
		const SizeInt& backbufferSize,
		std::span<Ref<Image>> backbuffers)
	{
		double aspectRatio = static_cast<double>(backbufferSize.Width) / backbufferSize.Height;
		const CameraComponent& camera = _entity.GetComponent<const CameraComponent>();
		Matrix4x4 projection = camera.GetProjectionMatrix(aspectRatio);
		Matrix4x4 view;
		Vector3 viewPosition;
		ViewFrustum frustum;
		MSAASamples sampleCount = pipeline.SupportsMSAA ? camera.SampleCount : MSAASamples::One;

		if (_entity.HasComponent<Transform3DComponent>())
		{
			const Transform3DComponent& cameraTransform = _entity.GetComponent<const Transform3DComponent>();
			view = cameraTransform.Transform.InvGlobalTransform;
			viewPosition = cameraTransform.Transform.GetGlobalPosition();
			frustum = camera.GetViewFrustum(aspectRatio, viewPosition, cameraTransform.Transform.GetGlobalRotation());
		}
		else
		{
			view = Matrix4x4::Identity;
			viewPosition = Vector3::Zero;
			frustum = camera.GetViewFrustum(aspectRatio, viewPosition, Quaternion::Identity);
		}

		RectInt viewport(Vector2Int::Zero, backbufferSize);

		std::vector<RenderTarget> rts = RenderService::Get()->GetAttachmentCache().CreateRenderTargets(pipeline, rendererID, backbufferSize, sampleCount, backbuffers);
		RenderTarget::SetClearValues(rts, camera.ClearColor, 1.0, 0);

		renderView.Setup(
			viewport, viewport,
			view,
			projection,
			viewPosition,
			frustum,
			sampleCount,
			rts
		);
	}

	void CameraSystem::Render(const Entity& camera, std::span<Ref<Image>> framebuffers, RenderPipeline& pipeline)
	{
		if (!camera.HasComponent<CameraComponent>())
			return;

		Assert(RenderService::Get() != nullptr)

		CameraRenderViewProvider cameraProvider(camera);
		SceneRender3DProvider sceneProvider(camera.GetScene());
		std::array<SceneDataProvider*, 1> sceneProviders = { &sceneProvider };
		RenderService::Get()->Render(camera.GetID(), framebuffers, pipeline, cameraProvider, sceneProviders);
	}
}