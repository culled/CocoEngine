#include "GamePanel.h"

#include <Coco/ECS/Components/Rendering/CameraSystem.h>
#include <Coco/Rendering/RenderService.h>
#include <Coco/Core/Engine.h>
#include <imgui.h>

using namespace Coco::ECS;
using namespace Coco::Rendering;

namespace Coco
{
	GamePanel::GamePanel(SharedRef<Scene> scene) :
		_currentScene(scene),
		_isOpen(true),
		_shouldFocus(false)
	{}

	GamePanel::~GamePanel()
	{}

	void GamePanel::SetCurrentScene(SharedRef<ECS::Scene> scene)
	{
		_currentScene = scene;
	}

	void GamePanel::Update(const TickInfo& tickInfo)
	{
		if (_shouldFocus)
		{
			ImGui::SetNextWindowFocus();
			_shouldFocus = false;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		if (ImGui::Begin("Game", nullptr, ImGuiWindowFlags_MenuBar))
		{
			_isOpen = true;

			UpdateWindowSettings();

			DrawViewportImage();
		}
		else
		{
			_isOpen = false;
		}

		ImGui::End();

		ImGui::PopStyleVar();
	}

	void GamePanel::Render(Rendering::RenderPipeline& pipeline)
	{
		if (!_isOpen || !_currentScene || !_viewportTexture)
			return;

		std::array<Ref<Image>, 1> cameraImages = {
			_viewportTexture->GetImage()
		};

		RenderService& rendering = *RenderService::Get();
		bool gizmos = rendering.GetRenderGizmos();
		rendering.SetGizmoRendering(false);

		CameraSystem::RenderScene(1, _currentScene, cameraImages, pipeline);

		rendering.SetGizmoRendering(gizmos);
	}

	void GamePanel::Focus()
	{
		_shouldFocus = true;
	}

	void GamePanel::UpdateWindowSettings()
	{
		ImVec2 pos = ImGui::GetWindowPos();
		ImVec2 min = ImGui::GetWindowContentRegionMin();
		pos.x += min.x;
		pos.y += min.y;

		ImVec2 size = ImGui::GetContentRegionAvail();
		_viewportRect.Minimum.X = static_cast<int>(pos.x);
		_viewportRect.Minimum.Y = static_cast<int>(pos.y);
		_viewportRect.Maximum.X = static_cast<int>(pos.x + size.x);
		_viewportRect.Maximum.Y = static_cast<int>(pos.y + size.y);
	}

	void GamePanel::DrawViewportImage()
	{
		SizeInt viewportSize = _viewportRect.GetSize();

		if (viewportSize.Width < 2 || viewportSize.Height < 2)
			return;

		EnsureViewportTexture();
		ImGui::Image(_viewportTexture.get(), ImVec2(static_cast<float>(viewportSize.Width), static_cast<float>(viewportSize.Height)));
	}

	void GamePanel::EnsureViewportTexture()
	{
		bool recreate = false;

		SizeInt size = _viewportRect.GetSize();

		if (!_viewportTexture)
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

		if (!_viewportTexture)
		{
			_viewportTexture = Engine::Get()->GetResourceLibrary().Create<Texture>(
				"Game Texture",
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
			_viewportTexture->Resize(size);
		}
	}
}