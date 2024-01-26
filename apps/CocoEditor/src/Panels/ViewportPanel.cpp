#include "ViewportPanel.h"

#include <Coco/ECS/ECSService.h>
#include <Coco/ECS/Components/Transform3DComponent.h>
#include <Coco/ECS/Components/Rendering/CameraComponent.h>
#include <Coco/ECS/Systems/Rendering/CameraSystem.h>
#include <Coco/ECS/Providers/Rendering/SceneRenderDataProvider.h>
#include <Coco/Rendering/RenderService.h>
#include <Coco/Input/InputService.h>
#include <Coco/Windowing/Window.h>

#include "../Components/ViewportCameraControllerComponent.h"

#include <imgui.h>

using namespace Coco::Rendering;
using namespace Coco::ECS;
using namespace Coco::Input;
using namespace Coco::Windowing;

namespace Coco
{
	ViewportPanel::ViewportPanel(SharedRef<Scene> scene, SelectionContext& selectionContext) :
		_selectionContext(selectionContext),
		_scene(scene),
		_framebuffer(nullptr),
		_isOpen(false),
		_isNavigating(false)
	{
		SetupViewportEntity();
	}

	ViewportPanel::~ViewportPanel()
	{
		ECSService::Get()->QueueDestroyEntity(_viewportEntity);
	}

	void ViewportPanel::SetScene(SharedRef<Scene> scene)
	{
		_scene = scene;
	}

	void ViewportPanel::Draw(const TickInfo& tickInfo)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		if (ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_MenuBar))
		{
			_isOpen = true;

			UpdateProperties();
			UpdateViewportCamera();
			UpdateFramebuffers();

			DrawFramebuffer();
		}
		else
		{
			_isOpen = false;
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void ViewportPanel::RenderFramebuffer(Rendering::RenderPipeline& pipeline)
	{
		if (!_isOpen)
			return;

		CameraRenderViewDataProvider renderViewProvider(_viewportEntity, std::optional<ShaderUniformLayout>());
		SceneRender3DDataProvider sceneProvider(_scene);

		std::array<SceneDataProvider*, 1> sceneProviders = { &sceneProvider };
		std::array<Ref<Image>, 1> framebuffers = { _framebuffer->GetImage() };

		std::hash<string> strHasher;

		// TODO: if multiple viewports, this renderID will need to change
		RenderService::Get()->Render(strHasher("Viewport"), framebuffers, pipeline, renderViewProvider, sceneProviders);
	}

	void ViewportPanel::SetupViewportEntity()
	{
		_viewportEntity = ECSService::Get()->CreateEntity("Viewport", Entity::Null, nullptr);

		Transform3DComponent& transform = _viewportEntity.AddComponent<Transform3DComponent>();
		transform.SetPositionAndRotation(Vector3(-2.0, 1.5, 2.0), Quaternion(Vector3(Math::DegToRad(-30), Math::DegToRad(-45), 0)), TransformSpace::Global);

		CameraComponent& camera = _viewportEntity.AddComponent<CameraComponent>();
		camera.SetClearColor(Color(0.1, 0.1, 0.1, 1.0));
		camera.SetPerspectiveFOV(Math::DegToRad(80.0));

		_viewportEntity.AddComponent<ViewportCameraControllerComponent>();
	}

	void ViewportPanel::UpdateProperties()
	{
		ImVec2 pos = ImGui::GetWindowPos();
		ImVec2 min = ImGui::GetWindowContentRegionMin();
		pos.x += min.x;
		pos.y += min.y;

		ImVec2 size = ImGui::GetContentRegionAvail();
		_viewportRect = RectInt(
			Vector2Int(static_cast<int>(pos.x), static_cast<int>(pos.y)),
			Vector2Int(static_cast<int>(pos.x + size.x), static_cast<int>(pos.y + size.y))
		);
	}

	void ViewportPanel::UpdateViewportCamera()
	{
		InputService& input = *InputService::Get();

		Mouse& mouse = input.GetMouse();

		Window* viewportWindow = reinterpret_cast<Window*>(ImGui::GetWindowViewport()->PlatformHandle);

		bool shouldNavigate = (mouse.IsButtonPressed(MouseButton::Right) || mouse.IsButtonPressed(MouseButton::Middle)) && ImGui::IsWindowHovered();

		if (shouldNavigate)
		{
			if (!_isNavigating)
			{
				viewportWindow->SetCursorConfineMode(CursorConfineMode::LockedInPlace);
				viewportWindow->SetCursorVisibility(false);
				_isNavigating = true;
			}

			ViewportCameraControllerComponent& controller = _viewportEntity.GetComponent<ViewportCameraControllerComponent>();
			controller.Navigate();
		}
		else if(_isNavigating)
		{
			viewportWindow->SetCursorConfineMode(CursorConfineMode::None);
			viewportWindow->SetCursorVisibility(true);
			_isNavigating = false;
		}
	}

	void ViewportPanel::UpdateFramebuffers()
	{
		SizeInt size = _viewportRect.GetSize();

		if (!_framebuffer)
		{
			_framebuffer = Engine::Get()->GetResourceLibrary().Create<Texture>(
				ResourceID("Viewport::Framebuffer"),
				ImageDescription::Create2D(
					size.Width,
					size.Height,
					ImagePixelFormat::RGBA8,
					ImageColorSpace::sRGB,
					ImageUsageFlags::RenderTarget | ImageUsageFlags::Sampled,
					false,
					MSAASamples::One
				),
				ImageSamplerDescription::LinearClamp
			);
		}
		else
		{
			_framebuffer->Resize(size.Width, size.Height);
		}
	}

	void ViewportPanel::DrawFramebuffer()
	{
		SizeInt size = _viewportRect.GetSize();

		if (size.Width < 2 || size.Height < 2)
			return;

		uint64 id = _framebuffer->GetID();
		ImGui::Image(reinterpret_cast<ImTextureID>(id), ImVec2(static_cast<float>(size.Width), static_cast<float>(size.Height)));
	}
}