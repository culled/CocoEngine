#include "CameraComponentUI.h"
#include "../../../Panels/ViewportPanel.h"

#include <imgui.h>

#include <Coco/Core/Engine.h>

using namespace Coco::ECS;
using namespace Coco::Rendering;

namespace Coco
{
	ViewportCameraComponentSettings::ViewportCameraComponentSettings(uint64 viewportID) :
		CameraPreviewScale(0.1f),
		CameraPreviewTexture(nullptr)
	{}

	std::unordered_map<uint64, ViewportCameraComponentSettings> CameraComponentUI::_settings;

	void CameraComponentUI::DrawInspectorUI(ECS::CameraComponent& camera)
	{
		std::array<const char*, 2> projectionTypeNames = { "Perspective", "Orthographic" };

		CameraProjectionType projectionType = camera.GetProjectionType();
		int projectionIndex = static_cast<int>(projectionType);

		if (ImGui::BeginCombo("Projection Type", projectionTypeNames.at(projectionIndex)))
		{
			for (int i = 0; i < projectionTypeNames.size(); i++)
			{
				bool selected = i == projectionIndex;

				if (ImGui::Selectable(projectionTypeNames.at(i), selected))
				{
					camera.SetProjectionType(static_cast<CameraProjectionType>(i));
				}
			}

			ImGui::EndCombo();
		}

		int priority = camera.GetPriority();
		if (ImGui::DragInt("Priority", &priority))
			camera.SetPriority(priority);

		std::array<float, 4> color = camera.GetClearColor().AsFloatArray(false);
		if (ImGui::ColorEdit3("Clear Color", color.data()))
		{
			camera.SetClearColor(Color(color, false));
		}

		switch (projectionType)
		{
		case CameraProjectionType::Perspective:
		{
			float fov = static_cast<float>(Math::RadToDeg(camera.GetPerspectiveFOV()));

			if (ImGui::DragFloat("Vertical FOV", &fov, 0.2f, Math::EpsilonF))
				camera.SetPerspectiveFOV(Math::DegToRad(fov));

			break;
		}
		case CameraProjectionType::Orthographic:
		{
			float size = static_cast<float>(camera.GetOrthographicSize());
			if (ImGui::DragFloat("Vertical Size", &size, 0.2f))
				camera.SetOrthographicSize(size);

			break;
		}
		default:
			break;
		}

		float near = static_cast<float>(camera.GetNearClip());
		float far = static_cast<float>(camera.GetFarClip());
		if (ImGui::DragFloatRange2("Clipping Distance", &near, &far, 0.1f, Math::EpsilonF))
		{
			camera.SetNearClip(near);
			camera.SetFarClip(far);
		}
	}

	bool CameraComponentUI::DrawViewport2D(ViewportPanel& viewport)
	{
		SelectionContext& selection = viewport.GetSelectionContext();
		if(!selection.HasSelectedEntity())
			return false;

		CameraComponent* camera = nullptr;
		if (!selection.GetSelectedEntity().TryGetComponent<CameraComponent>(camera))
			return false;

		ViewportCameraComponentSettings& settings = _settings.try_emplace(viewport.GetID(), viewport.GetID()).first->second;

		const RectInt viewportRect = viewport.GetViewportRect();
		SizeInt previewSize = viewportRect.GetSize();
		previewSize.Width = Math::Max(static_cast<int>(previewSize.Width * settings.CameraPreviewScale), 1);
		previewSize.Height = Math::Max(static_cast<int>(previewSize.Height * settings.CameraPreviewScale), 1);

		ImVec2 size(static_cast<float>(previewSize.Width), static_cast<float>(previewSize.Height));
		ImVec2 pos(static_cast<float>(viewportRect.Minimum.X), static_cast<float>(viewportRect.Minimum.Y));
		pos.x += viewportRect.GetSize().Width - (size.x + 40);
		pos.y += 30;
		ImGui::SetNextWindowPos(pos);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));

		bool hovered = false;

		// HACK: until the child window can auto-calculate its size based on its content, we force its size for now
		if (ImGui::BeginChild("Camera Preview",
			ImVec2(size.x + 10.f, size.y + 40),
			true,
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoNav |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoSavedSettings))
		{
			EnsureCameraPreviewTexture(viewport.GetID(), settings, previewSize);

			SharedRef<RenderPipeline> pipeline = viewport.GetRenderPipeline();
			std::array<Ref<Image>, 1> framebuffers = { settings.CameraPreviewTexture->GetImage() };
			camera->Render(viewport.GetID(), framebuffers, *pipeline);

			uint64 textureID = settings.CameraPreviewTexture->GetID();
			ImGui::Image(reinterpret_cast<ImTextureID>(textureID), size);

			//ImGui::Checkbox("Fullscreen", &_previewCameraFullscreen);

			hovered = ImGui::IsAnyItemHovered();
		}

		ImGui::EndChild();
		ImGui::PopStyleVar();

		return hovered;
	}

	void CameraComponentUI::EnsureCameraPreviewTexture(uint64 viewportID, ViewportCameraComponentSettings& settings, const SizeInt& previewSize)
	{
		if (!settings.CameraPreviewTexture)
		{
			settings.CameraPreviewTexture = Engine::Get()->GetResourceLibrary().Create<Texture>(
				ResourceID(FormatString("CameraPreviewTexture {}", viewportID)),
				ImageDescription::Create2D(
					previewSize.Width, previewSize.Height,
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
			settings.CameraPreviewTexture->Resize(previewSize.Width, previewSize.Height);
		}
	}
}