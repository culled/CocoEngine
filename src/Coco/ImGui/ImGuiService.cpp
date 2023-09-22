#include "ImGuiService.h"

#include <Coco/Windowing/WindowService.h>
#include <Coco/Rendering/RenderService.h>
#include <Coco/Core/Engine.h>

#include <vector>
#include <array>

#include <imgui.h>

namespace Coco::ImGui
{
	const int ImGuiService::sImGuiNewFramePriority = -100;
	const int ImGuiService::sImGuiDrawPriority = 100;

	ImGuiService::ImGuiService() :
		_newFrameTickListener(this, &ImGuiService::HandleNewFrameTick, sImGuiNewFramePriority),
		_drawTickListener(this, &ImGuiService::HandleDrawTick, sImGuiDrawPriority),
		_renderPass(CreateSharedRef<ImGuiRenderPass>())
	{
		using namespace Coco::Rendering;

		MainLoop::Get()->AddListener(_newFrameTickListener);
		MainLoop::Get()->AddListener(_drawTickListener);

		// Create the pipeline
		_renderPipeline = CreateUniqueRef<Rendering::RenderPipeline>();
		std::array<uint8, 1> bindings = { 0 };
		_renderPipeline->AddRenderPass(_renderPass, bindings);

		// Create the ImGui context and run setup
		::ImGui::CreateContext();
		_platform = CreateUniqueRef<ImGuiCocoPlatform>();

		CocoTrace("Created ImGuiService")
	}

	ImGuiService::~ImGuiService()
	{
		_renderPipeline.reset();
		_renderPass.reset();
		_platform.reset();

		MainLoop::Get()->RemoveListener(_newFrameTickListener);
		MainLoop::Get()->RemoveListener(_drawTickListener);

		::ImGui::DestroyContext();

		CocoTrace("Destroyed ImGuiService")
	}

	void ImGuiService::HandleNewFrameTick(const TickInfo& tickInfo)
	{
		if (!_platform->NewFrame(tickInfo))
			return;

		::ImGui::NewFrame();

		// HACK: temporary
		::ImGui::ShowDemoWindow();
	}

	void ImGuiService::HandleDrawTick(const TickInfo& tickInfo)
	{
		using namespace Coco::Windowing;
		using namespace Coco::Rendering;

		WindowService* windowing = WindowService::Get();
		if (!windowing)
			throw std::exception("No active WindowService found");

		Ref<Window> mainWindow = windowing->GetMainWindow();
		if (!mainWindow.IsValid())
			return;

		::ImGui::EndFrame();
		::ImGui::Render();

		RenderService* rendering = RenderService::Get();
		if (!rendering)
			throw std::exception("No active RenderService found");

		std::array<SceneDataProvider*, 1> sceneProviders = { _platform.get()};
		rendering->Render(mainWindow->GetPresenter(), *_renderPipeline, *_platform, sceneProviders);
	}
}