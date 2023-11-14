#include "ViewportPanel.h"

#include <Coco/Rendering/Graphics/ShaderUniformLayout.h>
#include <Coco/Rendering/RenderService.h>
#include <Coco/Input/InputService.h>
#include <Coco/Windowing/WindowService.h>
#include <Coco/Core/Engine.h>
#include <Coco/ECS/Providers/Rendering/SceneRenderProvider.h>
#include "../EditorApplication.h"
#include <Coco/ECS/Components/Rendering/CameraSystem.h>
#include <Coco/ECS/Components/Transform3DComponent.h>
#include "../UI/ComponentUI.h"
#include <Coco/Rendering/Gizmos/GizmoRender.h>

#include <imgui.h>
#include <ImGuizmo.h>

namespace Coco
{
	const float ViewportPanel::_sMinMoveSpeed = 0.01f;
	const float ViewportPanel::_sMaxMoveSpeed = 100;
	const double ViewportPanel::_sCameraPreviewSizePercentage = 0.15;

	ViewportPanel::ViewportPanel(const char* name, SharedRef<Scene> scene) :
		_name(name),
		_selection(EditorApplication::Get()->GetSelection()),
		_collapsed(true),
		_currentScene(scene),
		_sampleCount(MSAASamples::Four),
		_cameraTransform(Vector3(-2.0, 1.5, 2.0), Quaternion(Vector3(Math::DegToRad(-30), Math::DegToRad(-45), 0)), Vector3::One),
		_cameraComponent(Entity::Null),
		_lookSensitivity(0.5f),
		_moveSpeed(2.0),
		_panSpeed(2.0),
		_invertPan{false},
		_scrollDistance(0.2),
		_isNavigating(false),
		_isMouseHoveringGizmo(false),
		_isMouseHovering(false),
		_isMouseHoveringCameraPreview(false),
		_isFocused(false),
		_showCameraPreview(false),
		_previewCameraFullscreen(false),
		_drawGrid(true),
		_gridScale(1.0),
		_gridSquares(10),
		_enableSnap(false),
		_moveSnapIncrement(0.5f),
		_rotationSnapIncrement(45.f),
		_scaleSnapIncrement(0.5f),
		_cameraPreviewTexture(),
		_cameraPreviewSize(),
		_currentTransformMode(ImGuizmo::MODE::LOCAL),
		_currentTransformOperation(ImGuizmo::OPERATION::TRANSLATE)
	{
		_cameraComponent.SetClearColor(Color(0.1, 0.1, 0.1, 1.0));
		_cameraComponent.SetPerspectiveFOV(Math::DegToRad(80));
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
		Vector3 viewPosition = _cameraTransform.GetPosition(TransformSpace::Global);
		ViewFrustum frustum = _cameraComponent.GetViewFrustum(aspectRatio, viewPosition, _cameraTransform.GetRotation(TransformSpace::Global));

		RectInt viewport(Vector2Int::Zero, backbufferSize);

		std::vector<RenderTarget> rts = RenderService::Get()->GetAttachmentCache().CreateRenderTargets(pipeline, rendererID, backbufferSize, _sampleCount, backbuffers);
		RenderTarget::SetClearValues(rts, _cameraComponent.GetClearColor(), 1.0, 0);

		// Set the picking image clear value
		rts.back().ClearValue = Vector4(-1, -1, -1, -1);

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
		if (ImGui::Begin(_name.c_str(), nullptr, ImGuiWindowFlags_MenuBar))
		{
			DrawMenu();
		
			// Perform picking before our texture is possibly resized
			if (!ImGui::IsAnyItemActive() && 
				!_previewCameraFullscreen &&
				!_isMouseHoveringGizmo && 
				!_isMouseHoveringCameraPreview &&
				ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				PickEntity();
			}

			if (ImGui::IsKeyPressed(ImGuiKey_ModCtrl, false) || ImGui::IsKeyReleased(ImGuiKey_ModCtrl))
			{
				_enableSnap = !_enableSnap;
			}

			UpdateWindowSettings();

			if(_isMouseHovering)
				HandleShortcuts();

			DrawViewportImage();

			UpdateCamera(tickInfo);

			if (_selection.HasSelectedEntity())
			{
				DrawSelectedEntity();
			}
			else
			{
				_isMouseHoveringGizmo = false;
			}

			if (_drawGrid)
				DrawGrid();

			if (!_showCameraPreview)
			{
				_previewCameraFullscreen = false;
				_isMouseHoveringCameraPreview = false;
			}

			_collapsed = false;
		}
		else
		{
			_collapsed = true;
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* scenePayload = ImGui::AcceptDragDropPayload(".cscene"))
			{
				string filePathStr(static_cast<const char*>(scenePayload->Data), scenePayload->DataSize);
				EditorApplication::Get()->OpenScene(filePathStr);
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::End();
		ImGui::PopStyleVar();

		//if (!open)
		//	OnClosed.Invoke(*this);
	}

	void ViewportPanel::Render(RenderPipeline& pipeline)
	{
		if (_collapsed || !_viewportTexture)
			return;

		if (_showCameraPreview)
		{
			RenderService* rendering = RenderService::Get();
			bool gizmos = rendering->GetRenderGizmos();
			rendering->SetGizmoRendering(false);

			std::array<Ref<Image>, 1> cameraImages = { 
				_previewCameraFullscreen ? _viewportTexture->GetImage() : _cameraPreviewTexture->GetImage()
			};

			CameraComponent& camera = _selection.GetSelectedEntity().GetComponent<CameraComponent>();
			CameraSystem::Render(0, camera, cameraImages, pipeline);

			rendering->SetGizmoRendering(gizmos);
		}

		if (!_previewCameraFullscreen)
		{
			std::array<Ref<Image>, 2> viewportImages = { _viewportTexture->GetImage(), _viewportPickingTexture->GetImage() };

			SceneRender3DProvider sceneProvider(_currentScene);
			std::array<SceneDataProvider*, 1> sceneProviders = { &sceneProvider };
			RenderService::Get()->Render(0, viewportImages, pipeline, *this, sceneProviders);
		}
	}

	void ViewportPanel::DrawMenu()
	{
		if (ImGui::BeginMenuBar())
		{
			int operationV = static_cast<int>(_currentTransformOperation);

			ImGui::RadioButton("Translate", &operationV, 7);
			ImGui::RadioButton("Rotate", &operationV, 120);
			ImGui::RadioButton("Scale", &operationV, 896);

			_currentTransformOperation = static_cast<ImGuizmo::OPERATION>(operationV);

			ImGui::Separator();

			int modeV = static_cast<int>(_currentTransformMode);

			ImGui::RadioButton("Global", &modeV, 1);
			ImGui::RadioButton("Local", &modeV, 0);

			_currentTransformMode = static_cast<ImGuizmo::MODE>(modeV);

			ImGui::Separator();

			ImGui::Checkbox("Snap", &_enableSnap);

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.f, 10.f));

			if (ImGui::BeginMenu("Snap Settings"))
			{
				ImGui::DragFloat("Move Increment", &_moveSnapIncrement, 0.1f, 0.001f);
				ImGui::DragFloat("Rotation Increment", &_rotationSnapIncrement, 1.f, 0.001f, 180.f);
				ImGui::DragFloat("Scale Increment", &_scaleSnapIncrement, 0.1f, 0.001f);

				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::BeginMenu("View"))
			{
				ImGui::SeparatorText("Editor Camera Properties");

				ImGui::DragFloat("Mouse Sensitivity", &_lookSensitivity, 0.1f);
				ImGui::DragFloat("Move Speed", &_moveSpeed, 0.1f, _sMinMoveSpeed, _sMaxMoveSpeed);

				ImGui::DragFloat("Pan Speed", &_panSpeed, 0.1f);
				ImGui::Checkbox("Invert Pan X", &_invertPan.at(0));
				ImGui::SameLine();
				ImGui::Checkbox("Invert Pan Y", &_invertPan.at(1));

				std::array<const char*, 4> msaaTexts = { "One", "Two", "Four", "Eight" };

				if (ImGui::BeginCombo("MSAA samples", msaaTexts[static_cast<int>(_sampleCount)]))
				{
					for (int i = 0; i < 4; i++)
					{
						MSAASamples s = static_cast<MSAASamples>(i);
						if (ImGui::Selectable(msaaTexts[i], _sampleCount == s))
							_sampleCount = s;

					}

					ImGui::EndCombo();
				}

				float fov = static_cast<float>(Math::RadToDeg(_cameraComponent.GetPerspectiveFOV()));
				if (ImGui::DragFloat("Field of View", &fov, 0.1f, 0.01f, 180.f))
				{
					_cameraComponent.SetPerspectiveFOV(Math::DegToRad(fov));
				}

				float near = static_cast<float>(_cameraComponent.GetNearClip());
				float far = static_cast<float>(_cameraComponent.GetFarClip());
				if (ImGui::DragFloatRange2("Clipping Distance", &near, &far, 0.1f, Math::EpsilonF))
				{
					_cameraComponent.SetNearClip(near);
					_cameraComponent.SetFarClip(far);
				}

				ImGui::SeparatorText("Grid Properties");

				ImGui::Checkbox("Show Grid", &_drawGrid);
				ImGui::DragFloat("Grid Scale", &_gridScale, 0.1f);
				ImGui::DragInt("Grid Squares", &_gridSquares, 1.f, 1);

				ImGui::EndMenu();
			}

			ImGui::PopStyleVar();

			ImGui::EndMenuBar();
		}
	}

	void ViewportPanel::DrawViewportImage()
	{
		SizeInt viewportSize = _viewportRect.GetSize();

		if (viewportSize.Height < 2 || viewportSize.Width < 2)
			return;

		EnsureTexture(viewportSize, _viewportTexture);
		EnsurePickingTexture(viewportSize);
		ImGui::Image(_viewportTexture.get(), ImVec2(static_cast<float>(viewportSize.Width), static_cast<float>(viewportSize.Height)));
	}

	void ViewportPanel::DrawGrid()
	{
		Vector3 gridPos(Math::Round(_cameraTransform.LocalPosition.X), 0.0, Math::Round(_cameraTransform.LocalPosition.Z));

		GizmoRender::Get()->DrawGrid(gridPos, Quaternion::Identity, _gridScale * _gridSquares, _gridSquares, Color::MidGrey);
	}

	void ViewportPanel::EnsureTexture(const SizeInt& size, SharedRef<Texture>& texture)
	{
		bool recreate = false;

		if (!texture)
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

		if (!texture)
		{
			texture = Engine::Get()->GetResourceLibrary().Create<Texture>(
				"Viewport Texture",
				ImageDescription(
					size.Width, size.Height,
					ImagePixelFormat::RGBA8,
					ImageColorSpace::sRGB,
					ImageUsageFlags::RenderTarget | ImageUsageFlags::Sampled,
					false,
					MSAASamples::One),
				ImageSamplerDescription::LinearClamp);
		}
		else
		{
			texture->Resize(size);
		}
	}

	void ViewportPanel::EnsurePickingTexture(const SizeInt& size)
	{
		bool recreate = false;

		if (!_viewportPickingTexture)
		{
			recreate = true;
		}
		else
		{
			ImageDescription desc = _viewportPickingTexture->GetImage()->GetDescription();

			if (desc.Width != size.Width || desc.Height != size.Height)
				recreate = true;
		}

		if (!recreate)
			return;

		_viewportPickingTexture = Engine::Get()->GetResourceLibrary().Create<Texture>(
			"Viewport Picking Texture",
			ImageDescription(
				size.Width, size.Height,
				ImagePixelFormat::R32_Int,
				ImageColorSpace::Linear,
				ImageUsageFlags::RenderTarget | ImageUsageFlags::HostVisible,
				false,
				MSAASamples::One),
			ImageSamplerDescription::LinearClamp);
	}

	void ViewportPanel::UpdateWindowSettings()
	{
		_isMouseHovering = ImGui::IsWindowHovered();
		_isFocused = ImGui::IsWindowFocused();

		ImVec2 pos = ImGui::GetWindowPos();
		ImVec2 min = ImGui::GetWindowContentRegionMin();
		pos.x += min.x;
		pos.y += min.y;

		ImVec2 size = ImGui::GetContentRegionAvail();
		_viewportRect.Minimum.X = static_cast<int>(pos.x);
		_viewportRect.Minimum.Y = static_cast<int>(pos.y);
		_viewportRect.Maximum.X = static_cast<int>(pos.x + size.x);
		_viewportRect.Maximum.Y = static_cast<int>(pos.y + size.y);

		ImGuizmo::SetOrthographic(_cameraComponent.GetProjectionType() == CameraProjectionType::Orthographic);
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(pos.x, size.y + pos.y, size.x, -size.y);

		_cameraPreviewSize = SizeInt(
			static_cast<uint32>(size.x * _sCameraPreviewSizePercentage),
			static_cast<uint32>(size.y * _sCameraPreviewSizePercentage));
	}

	void ViewportPanel::HandleShortcuts()
	{
		using namespace Coco::Input;

		InputService& input = *InputService::Get();
		Mouse& mouse = input.GetMouse();
		Keyboard& keyboard = input.GetKeyboard();

		if (!_isNavigating)
		{
			int scrollDelta = mouse.GetScrollWheelDelta().Y;

			if (scrollDelta != 0)
			{
				_cameraTransform.Translate(Vector3::Forward * (scrollDelta * _scrollDistance), TransformSpace::Self);
				_cameraTransform.Recalculate();
			}
		}

		if (keyboard.WasKeyJustPressed(KeyboardKey::D1))
		{
			_currentTransformOperation = ImGuizmo::OPERATION::TRANSLATE;
		}

		if (keyboard.WasKeyJustPressed(KeyboardKey::D2))
		{
			_currentTransformOperation = ImGuizmo::OPERATION::ROTATE;
		}

		if (keyboard.WasKeyJustPressed(KeyboardKey::D3))
		{
			_currentTransformOperation = ImGuizmo::OPERATION::SCALE;
		}
	}

	void ViewportPanel::UpdateCamera(const TickInfo& tickInfo)
	{
		using namespace Coco::Input;
		using namespace Coco::Windowing;

		InputService& input = *InputService::Get();

		Mouse& mouse = input.GetMouse();

		Window* viewportWindow = reinterpret_cast<Window*>(ImGui::GetWindowViewport()->PlatformHandle);

		bool shouldNavigate = mouse.IsButtonPressed(MouseButton::Right) || mouse.IsButtonPressed(MouseButton::Middle);

		if (_isNavigating && !shouldNavigate)
		{
			viewportWindow->SetCursorConfineMode(CursorConfineMode::None);
			viewportWindow->SetCursorVisibility(true);
			_isNavigating = false;
			return;
		}
		else if (shouldNavigate && _isMouseHovering && !_previewCameraFullscreen)
		{
			_isNavigating = true;
		}

		int scrollDelta = mouse.GetScrollWheelDelta().Y;
		Keyboard& keyboard = input.GetKeyboard();

		if (_isNavigating)
		{
			viewportWindow->SetCursorConfineMode(CursorConfineMode::LockedInPlace);
			viewportWindow->SetCursorVisibility(false);

			bool shouldFly = mouse.IsButtonPressed(MouseButton::Right);

			Vector2Int mouseDelta = mouse.GetMoveDelta();

			if (shouldFly)
			{
				Vector3 eulerAngles = _cameraTransform.LocalRotation.ToEulerAngles();
				eulerAngles.X = Math::Clamp(eulerAngles.X - mouseDelta.Y * _lookSensitivity * 0.01, Math::DegToRad(-90.0), Math::DegToRad(90.0));
				eulerAngles.Y -= mouseDelta.X * _lookSensitivity * 0.01;

				_cameraTransform.LocalRotation = Quaternion(eulerAngles);

				if (scrollDelta != 0)
				{
					_moveSpeed = Math::Clamp(_moveSpeed + (_moveSpeed * scrollDelta * 0.2f), _sMinMoveSpeed, _sMaxMoveSpeed);
				}

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

				_cameraTransform.Translate(velocity * (tickInfo.DeltaTime * _moveSpeed), TransformSpace::Self);
			}
			else
			{
				Vector3 pan(-mouseDelta.X, mouseDelta.Y, 0.0);
				if (_invertPan.at(0))
					pan.X = -pan.X;

				if (_invertPan.at(1))
					pan.Y = -pan.Y;

				_cameraTransform.Translate(pan * (_panSpeed * 0.01), TransformSpace::Self);
			}

			_cameraTransform.Recalculate();

			ShowCameraStatsWindow();
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

		if (_previewCameraFullscreen)
			return;

		ComponentUI::DrawGizmos(e, _viewportRect.GetSize());

		if (e.HasComponent<Transform3DComponent>())
		{
			float snapIncrement = 0.f;

			switch (_currentTransformOperation)
			{
			case ImGuizmo::OPERATION::TRANSLATE:
				snapIncrement = _moveSnapIncrement;
				break;
			case ImGuizmo::OPERATION::ROTATE:
				snapIncrement = _rotationSnapIncrement;
				break;
			case ImGuizmo::OPERATION::SCALE:
				snapIncrement = _scaleSnapIncrement;
				break;
			default:
				break;
			}

			std::array<float, 3> snapValues = { 
				snapIncrement, 
				snapIncrement, 
				snapIncrement,
			};

			std::array<float, Matrix4x4::CellCount> cameraProjection = _cameraComponent.GetProjectionMatrix(_viewportRect.GetAspectRatio()).AsFloatArray();
			std::array<float, Matrix4x4::CellCount> cameraView = _cameraTransform.InvGlobalTransform.AsFloatArray();

			Transform3DComponent& transform = e.GetComponent<Transform3DComponent>();
			std::array<float, Matrix4x4::CellCount> model = transform.GetTransformMatrix(TransformSpace::Self, TransformSpace::Global).AsFloatArray();
			
			if (ImGuizmo::Manipulate(
				cameraView.data(),
				cameraProjection.data(),
				_currentTransformOperation,
				_currentTransformMode,
				model.data(),
				nullptr,
				_enableSnap ? snapValues.data() : nullptr))
			{
				Matrix4x4 transformed(model);

				Vector3 p, s;
				Quaternion r;
				transformed.Decompose(p, r, s);

				transform.SetPosition(p, TransformSpace::Global);
				transform.SetRotation(r, TransformSpace::Global);
				transform.SetScale(s, TransformSpace::Global);
			}

			_isMouseHoveringGizmo = ImGuizmo::IsOver();

			ImGuizmo::Enable(true);
		}
		else
		{
			_isMouseHoveringGizmo = false;
		}
	}

	void ViewportPanel::ShowCameraStatsWindow()
	{
		ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
		ImVec2 min(static_cast<float>(_viewportRect.Minimum.X), static_cast<float>(_viewportRect.Minimum.Y));
		min.x += 20;
		min.y += 30;
		ImGui::SetNextWindowPos(min);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));

		// HACK: until the child window can auto-calculate its size based on its content, we force its size for now
		if (ImGui::BeginChild("Stats", 
			ImVec2(200, 50), 
			true,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoNav |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoSavedSettings))
		{
			ImGui::Text("Position: %.3f, %.3f, %.3f", _cameraTransform.LocalPosition.X, _cameraTransform.LocalPosition.Y, _cameraTransform.LocalPosition.Z);
			ImGui::Text("Move speed: %.2f", _moveSpeed);
		}

		ImGui::EndChild();
		ImGui::PopStyleVar();
	}

	void ViewportPanel::ShowCameraPreview()
	{
		EnsureTexture(_cameraPreviewSize, _cameraPreviewTexture);
		ImVec2 previewSize{ static_cast<float>(_cameraPreviewSize.Width), static_cast<float>(_cameraPreviewSize.Height) };

		ImVec2 pos(static_cast<float>(_viewportRect.Minimum.X), static_cast<float>(_viewportRect.Minimum.Y));
		pos.x += _viewportRect.GetSize().Width - (previewSize.x + 40);
		pos.y += 30;
		ImGui::SetNextWindowPos(pos);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));

		// HACK: until the child window can auto-calculate its size based on its content, we force its size for now
		if (ImGui::BeginChild("Camera Preview",
			ImVec2(previewSize.x + 10.f, previewSize.y + 40),
			true,
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoNav |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoSavedSettings))
		{
			if(_viewportTexture)
				ImGui::Image(_previewCameraFullscreen ? _viewportTexture.get() : _cameraPreviewTexture.get(), previewSize);

			ImGui::Checkbox("Fullscreen", &_previewCameraFullscreen);

			_isMouseHoveringCameraPreview = ImGui::IsAnyItemHovered();
		}

		ImGui::EndChild();
		ImGui::PopStyleVar();
	}

	void ViewportPanel::PickEntity()
	{
		ImVec2 mousePos = ImGui::GetMousePos();
		Vector2Int viewportMousePos(
			static_cast<int>(mousePos.x),
			static_cast<int>(mousePos.y)
		);

		if (!_viewportRect.Intersects(viewportMousePos) || !_viewportPickingTexture)
			return;

		viewportMousePos.X -= _viewportRect.GetLeft();
		viewportMousePos.Y -= _viewportRect.GetBottom();

		int id = _viewportPickingTexture->ReadPixel<int>(viewportMousePos);

		Entity selectedEntity;
		if (id < 0 || !_currentScene->TryGetEntity(static_cast<EntityID>(id), selectedEntity))
		{
			_selection.ClearSelectedEntity();
			return;
		}
		
		_selection.SetSelectedEntity(selectedEntity);
		ImGuizmo::Enable(false);
	}
}