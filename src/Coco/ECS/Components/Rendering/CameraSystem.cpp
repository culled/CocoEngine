#include "ECSpch.h"
#include "CameraSystem.h"

#include "CameraComponent.h"
#include "../Transform3DComponent.h"
#include "../../Providers/Rendering/SceneRenderProvider.h"

#include <Coco/Rendering/Graphics/GraphicsPresenter.h>
#include <Coco/Rendering/RenderService.h>

namespace Coco::ECS
{
	CameraRenderViewProvider::CameraRenderViewProvider(const Entity& camera, std::optional<GlobalShaderUniformLayout> layoutOverride) :
		_entity(camera),
		_layoutOverride(layoutOverride)
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

		if (_entity.HasComponent<Transform3DComponent>())
		{
			const Transform3DComponent& cameraTransform = _entity.GetComponent<const Transform3DComponent>();
			Vector3 s;
			Quaternion r;
			cameraTransform.Decompose(TransformSpace::Global, viewPosition, r, s);

			view = cameraTransform.GetTransformMatrix(TransformSpace::Global, TransformSpace::Self);
			frustum = camera.GetViewFrustum(aspectRatio, viewPosition, r);
		}
		else
		{
			view = Matrix4x4::Identity;
			viewPosition = Vector3::Zero;
			frustum = camera.GetViewFrustum(aspectRatio, viewPosition, Quaternion::Identity);
		}

		RectInt viewport(Vector2Int::Zero, backbufferSize);

		std::vector<RenderTarget> rts = RenderService::Get()->GetAttachmentCache().CreateRenderTargets(pipeline, rendererID, backbufferSize, camera.GetSampleCount(), backbuffers);
		RenderTarget::SetClearValues(rts, camera.GetClearColor(), 1.0, 0);

		renderView.Setup(
			viewport, viewport,
			view,
			projection,
			viewPosition,
			frustum,
			camera.GetSampleCount(),
			rts
		);

		if (_layoutOverride.has_value())
			renderView.SetGlobalUniformLayout(_layoutOverride.value());
	}

	void CameraSystem::Render(
		const CameraComponent& camera,
		std::span<Ref<Image>> framebuffers,
		RenderPipeline& pipeline,
		std::optional<GlobalShaderUniformLayout> layoutOverride)
	{
		Assert(RenderService::Get() != nullptr)

		const Entity& cameraEntity = camera.GetOwner();
		CameraRenderViewProvider cameraProvider(cameraEntity, layoutOverride);
		SceneRender3DProvider sceneProvider(cameraEntity.GetScene());
		std::array<SceneDataProvider*, 1> sceneProviders = { &sceneProvider };
		RenderService::Get()->Render(cameraEntity.GetID(), framebuffers, pipeline, cameraProvider, sceneProviders);
	}

	void CameraSystem::Render(
		const CameraComponent& camera,
		Ref<GraphicsPresenter> presenter, 
		RenderPipeline& pipeline,
		std::optional<GlobalShaderUniformLayout> layoutOverride)
	{
		Assert(RenderService::Get() != nullptr)

		const Entity& cameraEntity = camera.GetOwner();
		CameraRenderViewProvider cameraProvider(cameraEntity, layoutOverride);
		SceneRender3DProvider sceneProvider(cameraEntity.GetScene());
		std::array<SceneDataProvider*, 1> sceneProviders = { &sceneProvider };
		RenderService::Get()->Render(presenter, pipeline, cameraProvider, sceneProviders);
	}
}