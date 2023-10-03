#include "ViewportPanel.h"

#include <Coco/Rendering/Graphics/ShaderUniformLayout.h>
#include <Coco/Rendering/RenderService.h>
#include <Coco/Input/InputService.h>
#include <Coco/Windowing/WindowService.h>
#include <Coco/Core/Engine.h>

#include <imgui.h>

namespace Coco
{
	ViewportPanel::ViewportPanel(const char* name) :
		_name(name),
		_collapsed(true),
		_clearColor(Color(0.1, 0.1, 0.1, 1.0)),
		_sampleCount(MSAASamples::One),
		_verticalFOV(90.0),
		_cameraTransform(Vector3(0.0, 0.0, 1.0), Quaternion::Identity, Vector3::One),
		_lookSensitivity(0.005),
		_moveSpeed(2.0),
		_isFlying(false),
		_attachmentCache(CreateUniqueRef<AttachmentCache>()),
		_updateTickListener(CreateManagedRef<TickListener>(this, &ViewportPanel::Update, 10))
	{
		MainLoop::Get()->AddListener(_updateTickListener);
	}

	ViewportPanel::~ViewportPanel()
	{
		_attachmentCache.reset();

		MainLoop::Get()->RemoveListener(_updateTickListener);
	}

	void ViewportPanel::SetupRenderView(
		RenderView& renderView, 
		const CompiledRenderPipeline& pipeline, 
		uint64 rendererID, 
		const SizeInt& backbufferSize, 
		std::span<Ref<Image>> backbuffers)
	{
		std::vector<RenderTarget> rts = _attachmentCache->CreateRenderTargets(pipeline, rendererID, backbufferSize, _sampleCount, backbuffers);
		RenderTarget::SetClearValues(rts, _clearColor, 1.0, 0);

		double aspectRatio = static_cast<double>(backbufferSize.Width) / backbufferSize.Height;
		RectInt viewport(Vector2Int::Zero, backbufferSize);
		Matrix4x4 view = _cameraTransform.InvGlobalTransform;
		Matrix4x4 projection = RenderService::Get()->GetPlatform().CreatePerspectiveProjection(Math::DegToRad(_verticalFOV), aspectRatio, 0.1, 100);
		ViewFrustum frustum = ViewFrustum::CreatePerspective(
			_cameraTransform.GetGlobalPosition(),
			_cameraTransform.GetGlobalBackward(),
			_cameraTransform.GetGlobalUp(),
			Math::DegToRad(_verticalFOV),
			aspectRatio,
			0.1, 100);

		renderView.Setup(
			viewport,
			viewport,
			view,
			projection,
			_cameraTransform.GetGlobalPosition(),
			frustum,
			pipeline.SupportsMSAA ? _sampleCount : MSAASamples::One,
			rts);
	}

	void ViewportPanel::Render(RenderPipeline& pipeline, std::span<SceneDataProvider*> sceneDataProviders)
	{
		if (_collapsed)
			return;

		std::array<Ref<Image>, 1> images = { _viewportTexture->GetImage() };
		RenderService::Get()->Render(0, images, pipeline, *this, sceneDataProviders);
	}

	void ViewportPanel::Update(const TickInfo& tickInfo)
	{
		bool open = true;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		if (ImGui::Begin(_name.c_str(), &open) && open)
		{
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

		if (!open)
			OnClosed.Invoke(*this);
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
		if (!mouse.IsButtonPressed(MouseButton::Right))
		{
			mainWindow->SetCursorConfineMode(CursorConfineMode::None);
			mainWindow->SetCursorVisibility(true);
			_isFlying = false;
			return;
		}
		else if (mouse.WasButtonJustPressed(MouseButton::Right) && ImGui::IsWindowHovered())
		{
			_isFlying = true;
		}

		if (!_isFlying)
			return;

		mainWindow->SetCursorConfineMode(CursorConfineMode::Locked);
		mainWindow->SetCursorVisibility(false);

		Vector2Int mouseDelta = mouse.GetMoveDelta();
		Vector3 eulerAngles = _cameraTransform.LocalRotation.ToEulerAngles();
		eulerAngles.X = Math::Clamp(eulerAngles.X - mouseDelta.Y * _lookSensitivity, Math::DegToRad(-90.0), Math::DegToRad(90.0));
		eulerAngles.Y -= mouseDelta.X * _lookSensitivity;

		_cameraTransform.LocalRotation = Quaternion(eulerAngles);

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
		_cameraTransform.TranslateLocal(velocity * tickInfo.DeltaTime);
		_cameraTransform.Recalculate();
	}
}