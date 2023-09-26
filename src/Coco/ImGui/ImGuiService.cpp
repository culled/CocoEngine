#include "ImGuiService.h"

#include <Coco/Windowing/WindowService.h>
#include <Coco/Rendering/RenderService.h>
#include <Coco/Core/Engine.h>

#include <vector>
#include <array>

#include <imgui.h>

namespace Coco::ImGuiCoco
{
	const int ImGuiService::sImGuiNewFramePriority = -100;
	const int ImGuiService::sImGuiDrawPriority = 100;

	ImGuiService::ImGuiService() :
		_newFrameTickListener(this, &ImGuiService::HandleNewFrameTick, sImGuiNewFramePriority),
		_drawTickListener(this, &ImGuiService::HandleDrawTick, sImGuiDrawPriority)
	{
		using namespace Coco::Rendering;

		MainLoop::Get()->AddListener(_newFrameTickListener);
		MainLoop::Get()->AddListener(_drawTickListener);

		// Create the ImGui context and run setup
		::ImGui::CreateContext();
		_platform = CreateUniqueRef<ImGuiCocoPlatform>();

		CocoTrace("Created ImGuiService")
	}

	ImGuiService::~ImGuiService()
	{
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

		const ImGuiIO& io = ::ImGui::GetIO();
		if (io.ConfigFlags && ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
		}

		ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();

		for (int i = 0; i < platformIO.Viewports.Size; i++)
		{
			ImGuiViewport* viewport = platformIO.Viewports[i];
			if (viewport->Flags & ImGuiViewportFlags_IsMinimized)
				continue;

			if (platformIO.Platform_RenderWindow) platformIO.Platform_RenderWindow(viewport, nullptr);
		}
	}
}