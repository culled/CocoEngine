#include "ECSpch.h"
#include "CameraSystem.h"

#include "../../ComponentView.h"
#include "../../Components/EntityInfoComponent.h"
#include "../../Components/Rendering/CameraComponent.h"
#include "../../Components/Transform3DComponent.h"
#include "../../Providers/Rendering/SceneRenderDataProvider.h"

#include <Coco/Rendering/Graphics/Presenter.h>
#include <Coco/Rendering/RenderService.h>

namespace Coco::ECS
{
	CameraRenderViewDataProvider::CameraRenderViewDataProvider(const Entity& camera, std::optional<ShaderUniformLayout> layoutOverride) :
		_entity(camera),
		_layoutOverride(layoutOverride)
	{}

	void CameraRenderViewDataProvider::SetupRenderView(
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

		std::vector<RenderTarget> rts = RenderService::Get()->GetAttachmentCache().GetRenderTargets(pipeline, rendererID, backbufferSize, camera.GetSampleCount(), backbuffers);
		RenderTarget::SetClearValues(rts, camera.GetClearColor(), 1.0, 0);

		renderView.Setup(
			rts,
			viewport,
			view,
			projection,
			frustum,
			camera.GetSampleCount(),
			_layoutOverride
		);
	}

	void CameraSystem::Render(
		uint64 rendererID,
		const CameraComponent& camera,
		std::span<Ref<Image>> framebuffers,
		RenderPipeline& pipeline,
		std::optional<ShaderUniformLayout> layoutOverride)
	{
		Assert(RenderService::Get() != nullptr)

		const Entity& cameraEntity = camera.GetOwner();
		const EntityInfoComponent& entityInfo = cameraEntity.GetComponent<EntityInfoComponent>();
		CameraRenderViewDataProvider cameraProvider(cameraEntity, layoutOverride);
		SceneRender3DDataProvider sceneProvider(entityInfo.GetScene());

		std::array<SceneDataProvider*, 1> sceneProviders = { &sceneProvider };

		RenderService::Get()->Render(rendererID, framebuffers, pipeline, cameraProvider, sceneProviders);
	}

	void CameraSystem::Render(
		uint64 rendererID,
		const CameraComponent& camera,
		Ref<Presenter> presenter, 
		RenderPipeline& pipeline,
		std::optional<ShaderUniformLayout> layoutOverride)
	{
		Assert(RenderService::Get() != nullptr)

		const Entity& cameraEntity = camera.GetOwner();
		const EntityInfoComponent& entityInfo = cameraEntity.GetComponent<EntityInfoComponent>();
		CameraRenderViewDataProvider cameraProvider(cameraEntity, layoutOverride);
		SceneRender3DDataProvider sceneProvider(entityInfo.GetScene());

		std::array<SceneDataProvider*, 1> sceneProviders = { &sceneProvider };

		RenderService::Get()->Render(rendererID, presenter, pipeline, cameraProvider, sceneProviders);
	}

	bool CameraSystem::RenderScene(
		uint64 rendererID, 
		SharedRef<Scene> scene, 
		std::span<Ref<Image>> framebuffers, 
		RenderPipeline& pipeline, 
		std::optional<ShaderUniformLayout> layoutOverride)
	{
		CameraComponent* activeCamera = nullptr;

		if (!TryGetActiveCamera(scene, activeCamera))
			return false;

		Render(rendererID, *activeCamera, framebuffers, pipeline, layoutOverride);

		return true;
	}

	bool CameraSystem::RenderScene(
		uint64 rendererID, 
		SharedRef<Scene> scene, 
		Ref<Presenter> presenter, 
		RenderPipeline& pipeline, 
		std::optional<ShaderUniformLayout> layoutOverride)
	{
		CameraComponent* activeCamera = nullptr;

		if (!TryGetActiveCamera(scene, activeCamera))
			return false;

		Render(rendererID, *activeCamera, presenter, pipeline, layoutOverride);

		return true;
	}

	bool CameraSystem::TryGetActiveCamera(SharedRef<Scene> scene, CameraComponent*& outActiveCamera)
	{
		ComponentView<CameraComponent> view(scene);

		CameraComponent* currentCamera = nullptr;

		for (Entity& e : view)
		{
			if (!e.IsActiveInHierarchy())
				continue;

			CameraComponent& camera = e.GetComponent<CameraComponent>();

			if (currentCamera == nullptr || camera.GetPriority() < currentCamera->GetPriority())
				currentCamera = &camera;
		}

		if (!currentCamera)
			return false;

		outActiveCamera = currentCamera;
		return true;
	}
}