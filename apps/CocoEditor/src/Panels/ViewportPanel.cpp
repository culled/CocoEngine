#include "ViewportPanel.h"

#include <Coco/Rendering/Graphics/ShaderUniformLayout.h>
#include <Coco/Rendering/RenderService.h>
#include <Coco/Input/InputService.h>
#include <Coco/Windowing/WindowService.h>
#include <Coco/Core/Engine.h>
#include <Coco/ECS/Systems/Rendering/SceneRenderProvider.h>

#include <imgui.h>

namespace Coco
{
	const double ViewportPanel::_sMinMoveSpeed = 0.01;
	const double ViewportPanel::_sMaxMoveSpeed = 100;

	ViewportPanel::ViewportPanel(const char* name, SharedRef<Scene> scene) :
		_name(name),
		_collapsed(true),
		_currentScene(scene),
		_sampleCount(MSAASamples::One),
		_cameraTransform(Vector3(0.0, 0.0, 1.0), Quaternion::Identity, Vector3::One),
		_cameraComponent(),
		_lookSensitivity(0.005),
		_moveSpeed(2.0),
		_scrollDistance(0.2),
		_isFlying(false),
		_isMouseHovering(false),
		_isFocused(false)
	{
		_cameraComponent.ClearColor = Color(0.1, 0.1, 0.1, 1.0);
	}

	ViewportPanel::~ViewportPanel()
	{
	}

	void ViewportPanel::SetupRenderView(
		RenderView& renderView, 
		const CompiledRenderPipeline& pipeline, 
		uint64 rendererID, 
		const SizeInt& backbufferSize, 
		std::span<Ref<Image>> backbuffers)
	{
		double aspectRatio = static_cast<double>(backbufferSize.Width) / backbufferSize.Height;
		Matrix4x4 projection = _cameraComponent.GetProjectionMatrix(aspectRatio);
		Matrix4x4 view = _cameraTransform.InvGlobalTransform;
		Vector3 viewPosition = _cameraTransform.GetGlobalPosition();
		ViewFrustum frustum = _cameraComponent.GetViewFrustum(aspectRatio, viewPosition, _cameraTransform.GetGlobalRotation());

		RectInt viewport(Vector2Int::Zero, backbufferSize);

		std::vector<RenderTarget> rts = RenderService::Get()->GetAttachmentCache().CreateRenderTargets(pipeline, rendererID, backbufferSize, _sampleCount, backbuffers);
		RenderTarget::SetClearValues(rts, _cameraComponent.ClearColor, 1.0, 0);

		renderView.Setup(
			viewport, viewport,
			view,
			projection,
			viewPosition,
			frustum,
			_sampleCount,
			rts
		);
	}

	void ViewportPanel::SetCurrentScene(SharedRef<Scene> scene)
	{
		_currentScene = scene;
	}

	void ViewportPanel::Update(const TickInfo& tickInfo)
	{
		bool open = true;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		//if (ImGui::Begin(_name.c_str(), &open) && open)
		if (ImGui::Begin(_name.c_str()))
		{
			_isMouseHovering = ImGui::IsWindowHovered();
			_isFocused = ImGui::IsWindowFocused();

			UpdateCamera(tickInfo);

			ImVec2 size = ImGui::GetContentRegionAvail();
			EnsureViewportTexture(SizeInt(static_cast<uint32>(size.x), static_cast<uint32>(size.y)));

			ImGui::Image(_viewportTexture.Get(), size);

			_collapsed = false;
		}
		else
		{
			_collapsed = true;
		}

		ImGui::End();
		ImGui::PopStyleVar();

		//if (!open)
		//	OnClosed.Invoke(*this);
	}

	void ViewportPanel::Render(RenderPipeline& pipeline)
	{
		if (_collapsed)
			return;

		std::array<Ref<Image>, 1> images = { _viewportTexture->GetImage() };
		SceneRender3DProvider sceneProvider(_currentScene);
		std::array<SceneDataProvider*, 1> sceneProviders = { &sceneProvider };
		RenderService::Get()->Render(0, images, pipeline, *this, sceneProviders);
	}

	void ViewportPanel::EnsureViewportTexture(const SizeInt& size)
	{
		bool recreate = false;

		if (!_viewportTexture.IsValid())
		{
			recreate = true;
		}
		else
		{
			ImageDescription desc = _viewportTexture->GetImage()->GetDescription();

			if (desc.Width != size.Width || desc.Height != size.Height)
				recreate = true;
		}

		if (!recreate)
			return;

		_viewportTexture = CreateManagedRef<Texture>(
			0,
			"ViewportPanel Backbuffer",
			ImageDescription(
				size.Width, size.Height,
				ImagePixelFormat::RGBA8,
				ImageColorSpace::sRGB,
				ImageUsageFlags::RenderTarget | ImageUsageFlags::Sampled,
				false,
				_sampleCount),
			ImageSamplerDescription::LinearClamp);
	}

	void ViewportPanel::UpdateCamera(const TickInfo& tickInfo)
	{
		using namespace Coco::Input;
		using namespace Coco::Windowing;

		InputService& input = *InputService::Get();

		Mouse& mouse = input.GetMouse();

		Ref<Window> mainWindow = WindowService::Get()->GetMainWindow();
		if (_isFlying && !mouse.IsButtonPressed(MouseButton::Right))
		{
			mainWindow->SetCursorConfineMode(CursorConfineMode::None);
			mainWindow->SetCursorVisibility(true);
			_isFlying = false;
			return;
		}
		else if (mouse.WasButtonJustPressed(MouseButton::Right) && _isMouseHovering)
		{
			_isFlying = true;
		}

		int scrollDelta = mouse.GetScrollWheelDelta().Y;

		if (_isFlying)
		{
			mainWindow->SetCursorConfineMode(CursorConfineMode::LockedInPlace);
			mainWindow->SetCursorVisibility(false);

			Vector2Int mouseDelta = mouse.GetMoveDelta();
			Vector3 eulerAngles = _cameraTransform.LocalRotation.ToEulerAngles();
			eulerAngles.X = Math::Clamp(eulerAngles.X - mouseDelta.Y * _lookSensitivity, Math::DegToRad(-90.0), Math::DegToRad(90.0));
			eulerAngles.Y -= mouseDelta.X * _lookSensitivity;

			_cameraTransform.LocalRotation = Quaternion(eulerAngles);

			if (scrollDelta != 0)
			{
				_moveSpeed = Math::Clamp(_moveSpeed + (_moveSpeed * scrollDelta * 0.2), _sMinMoveSpeed, _sMaxMoveSpeed);
			}

			Keyboard& keyboard = input.GetKeyboard();

			Vector3 velocity = Vector3::Zero;

			if (keyboard.IsKeyPressed(KeyboardKey::W))
				velocity += Vector3::Forward;

			if (keyboard.IsKeyPressed(KeyboardKey::S))
				velocity += Vector3::Backward;

			if (keyboard.IsKeyPressed(KeyboardKey::D))
				velocity += Vector3::Right;

			if (keyboard.IsKeyPressed(KeyboardKey::A))
				velocity += Vector3::Left;

			if (keyboard.IsKeyPressed(KeyboardKey::E))
				velocity += Vector3::Up;

			if (keyboard.IsKeyPressed(KeyboardKey::Q))
				velocity += Vector3::Down;

			velocity = _cameraTransform.LocalRotation * velocity;
			_cameraTransform.TranslateLocal(velocity * (tickInfo.DeltaTime * _moveSpeed));
			_cameraTransform.Recalculate();

			ShowCameraStatsWindow();
		}
		else if (_isMouseHovering && scrollDelta != 0)
		{
			_cameraTransform.TranslateGlobal(_cameraTransform.GetGlobalBackward() * (scrollDelta * _scrollDistance));
			_cameraTransform.Recalculate();
		}
	}

	void ViewportPanel::ShowCameraStatsWindow()
	{
		ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
		ImVec2 min = ImGui::GetWindowPos();
		min.x += 20;
		min.y += 30;
		ImGui::SetNextWindowPos(min);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));

		ImGui::Begin("Stats", nullptr,
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoNav |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoSavedSettings);

		ImGui::Text("Position: %.3f, %.3f, %.3f", _cameraTransform.LocalPosition.X, _cameraTransform.LocalPosition.Y, _cameraTransform.LocalPosition.Z);
		ImGui::Text("Move speed: %.2f", _moveSpeed);

		ImGui::End();
		ImGui::PopStyleVar();
	}
}