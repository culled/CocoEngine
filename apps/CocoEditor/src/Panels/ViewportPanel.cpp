#include "ViewportPanel.h"

#include <Coco/Rendering/Graphics/ShaderUniformLayout.h>
#include <Coco/Rendering/RenderService.h>
#include <Coco/Input/InputService.h>
#include <Coco/Windowing/WindowService.h>
#include <Coco/Core/Engine.h>
#include <Coco/ECS/Systems/Rendering/SceneRenderProvider.h>
#include "../EditorApplication.h"
#include <Coco/ECS/Systems/Rendering/CameraSystem.h>
#include <Coco/ECS/Components/Transform3DComponent.h>
#include "../UI/ComponentUI.h"
#include <Coco/ECS/Systems/TransformSystem.h>

#include <imgui.h>
#include <ImGuizmo.h>

namespace Coco
{
	const double ViewportPanel::_sMinMoveSpeed = 0.01;
	const double ViewportPanel::_sMaxMoveSpeed = 100;
	const double ViewportPanel::_sCameraPreviewSizePercentage = 0.15;

	ViewportPanel::ViewportPanel(const char* name, SharedRef<Scene> scene) :
		_name(name),
		_selection(EditorApplication::Get()->GetSelection()),
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
		_isFocused(false),
		_showCameraPreview(false),
		_previewCameraFullscreen(false),
		_cameraPreviewTexture(),
		_cameraPreviewSize()
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
		_showCameraPreview = false;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		//if (ImGui::Begin(_name.c_str(), &open) && open)
		if (ImGui::Begin(_name.c_str()))
		{
			_isMouseHovering = ImGui::IsWindowHovered();
			_isFocused = ImGui::IsWindowFocused();

			UpdateCamera(tickInfo);

			ImVec2 pos = ImGui::GetWindowPos();
			ImVec2 size = ImGui::GetContentRegionAvail(); 
			_viewportRect.Minimum.X = static_cast<int>(pos.x);
			_viewportRect.Minimum.Y = static_cast<int>(pos.y);
			_viewportRect.Maximum.X = static_cast<int>(pos.x + size.x);
			_viewportRect.Maximum.Y = static_cast<int>(pos.y + size.y); 
			
			ImGuizmo::SetOrthographic(_cameraComponent.ProjectionType == CameraProjectionType::Orthographic);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(pos.x, pos.y + size.y, size.x, -size.y);

			_cameraPreviewSize = SizeInt(
				static_cast<uint32>(size.x * _sCameraPreviewSizePercentage),
				static_cast<uint32>(size.y * _sCameraPreviewSizePercentage));

			if (!_showCameraPreview)
				_previewCameraFullscreen = false;

			EnsureTexture(_viewportRect.GetSize(), _viewportTexture);

			ImGui::Image(_viewportTexture.Get(), size);

			if (_selection.HasSelectedEntity())
			{
				DrawSelectedEntity();
			}

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

		std::array<Ref<Image>, 1> viewportImages = { _viewportTexture->GetImage() };

		if (_showCameraPreview)
		{
			std::array<Ref<Image>, 1> cameraImages = { _cameraPreviewTexture->GetImage() };
			CameraSystem::Render(_selection.GetSelectedEntity(), cameraImages, pipeline);

			if (_previewCameraFullscreen)
			{
				CameraSystem::Render(_selection.GetSelectedEntity(), viewportImages, pipeline);
			}
		}

		if (!_previewCameraFullscreen)
		{
			SceneRender3DProvider sceneProvider(_currentScene);
			std::array<SceneDataProvider*, 1> sceneProviders = { &sceneProvider };
			RenderService::Get()->Render(0, viewportImages, pipeline, *this, sceneProviders);
		}
	}

	void ViewportPanel::EnsureTexture(const SizeInt& size, ManagedRef<Texture>& texture)
	{
		bool recreate = false;

		if (!texture.IsValid())
		{
			recreate = true;
		}
		else
		{
			ImageDescription desc = texture->GetImage()->GetDescription();

			if (desc.Width != size.Width || desc.Height != size.Height)
				recreate = true;
		}

		if (!recreate)
			return;

		texture = CreateManagedRef<Texture>(
			0,
			"Viewport Texture",
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
		else if (mouse.WasButtonJustPressed(MouseButton::Right) && _isMouseHovering && !_previewCameraFullscreen)
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

	void ViewportPanel::DrawSelectedEntity()
	{
		Entity& e = _selection.GetSelectedEntity();

		if (e.HasComponent<CameraComponent>())
		{
			_showCameraPreview = true;
			ShowCameraPreview();
		}

		ComponentUI::DrawGizmos(e, _viewportRect.GetSize());

		if (e.HasComponent<Transform3DComponent>())
		{
			std::array<float, Matrix4x4::CellCount> cameraProjection = _cameraComponent.GetProjectionMatrix(_viewportRect.GetAspectRatio()).AsFloatArray();
			std::array<float, Matrix4x4::CellCount> cameraView = _cameraTransform.InvGlobalTransform.AsFloatArray();

			// Ensure our transform is updated
			TransformSystem::UpdateTransform3D(e);
			Transform3DComponent& transformComp = e.GetComponent<Transform3DComponent>();
			Transform3D& transform = transformComp.Transform;
			std::array<float, Matrix4x4::CellCount> model = transform.GlobalTransform.AsFloatArray();

			if (ImGuizmo::Manipulate(
				cameraView.data(),
				cameraProjection.data(),
				ImGuizmo::OPERATION::TRANSLATE | ImGuizmo::OPERATION::ROTATE | ImGuizmo::OPERATION::SCALE,
				ImGuizmo::MODE::LOCAL,
				model.data()))
			{
				Matrix4x4 transformed;
				for (size_t i = 0; i < Matrix4x4::CellCount; i++)
					transformed.Data[i] = model.at(i);

				transformed.Decompose(transform.LocalPosition, transform.LocalRotation, transform.LocalScale);

				Transform3DComponent* parentTransformComp = nullptr;
				if (TransformSystem::TryGetParentTransform3D(e, parentTransformComp))
				{
					parentTransformComp->Transform.TransformGlobalToLocal(transform.LocalPosition, transform.LocalRotation, transform.LocalScale);
				}

				TransformSystem::MarkTransform3DDirty(e);
			}
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

	void ViewportPanel::ShowCameraPreview()
	{
		EnsureTexture(_cameraPreviewSize, _cameraPreviewTexture);
		ImVec2 previewSize{ static_cast<float>(_cameraPreviewSize.Width), static_cast<float>(_cameraPreviewSize.Height) };

		ImVec2 pos = ImGui::GetWindowPos();
		pos.x += _viewportRect.GetSize().Width - (previewSize.x + 30);
		pos.y += 30;
		ImGui::SetNextWindowPos(pos);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));

		if (ImGui::Begin("Camera Preview", nullptr,
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoNav |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoSavedSettings))
		{

			ImGui::Image(_cameraPreviewTexture.Get(), previewSize);

			ImGui::Checkbox("Fullscreen", &_previewCameraFullscreen);
		}

		ImVec2 size = ImGui::GetWindowSize();
		pos.x -= size.x - previewSize.x;
		ImGui::SetWindowPos(pos);

		ImGui::End();
		ImGui::PopStyleVar();
	}
}