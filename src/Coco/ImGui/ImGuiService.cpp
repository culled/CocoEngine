#include "ImGuiService.h"

#include <Coco/Windowing/WindowService.h>
#include <Coco/Rendering/RenderService.h>
#include <Coco/Input/InputService.h>
#include <Coco/Core/Engine.h>

#include <vector>
#include <array>

#include <imgui.h>
#include <ImGuizmo.h>

namespace Coco::ImGuiCoco
{
	const int ImGuiService::sImGuiNewFramePriority = -900;
	const int ImGuiService::sImGuiDrawPriority = 900;

	bool ImGuiWantsMouse(bool &wantsMouse)
	{
		ImGuiIO& io = ImGui::GetIO();
		wantsMouse = io.WantCaptureMouse;

		return true;
	}

	bool ImGuiWantsKeyboard(bool& wantsKeyboard)
	{
		ImGuiIO& io = ImGui::GetIO();
		wantsKeyboard = io.WantCaptureKeyboard;

		return true;
	}

	ImGuiService::ImGuiService(bool enableViewports) :
		HasMouseCapture(&ImGuiWantsMouse),
		HasKeyboardCapture(&ImGuiWantsKeyboard),
		_newFrameTickListener(CreateManagedRef<TickListener>(this, &ImGuiService::HandleNewFrameTick, sImGuiNewFramePriority)),
		_drawTickListener(CreateManagedRef<TickListener>(this, &ImGuiService::HandleDrawTick, sImGuiDrawPriority))
	{
		using namespace Coco::Rendering;

		MainLoop::Get()->AddListener(_newFrameTickListener);
		MainLoop::Get()->AddListener(_drawTickListener);

		// Create the ImGui context and run setup
		::ImGui::CreateContext();
		_platform = CreateUniqueRef<ImGuiCocoPlatform>(enableViewports);

		CocoTrace("Created ImGuiService")
	}

	ImGuiService::~ImGuiService()
	{
		_inputLayer.Invalidate();

		_platform.reset();

		MainLoop::Get()->RemoveListener(_newFrameTickListener);
		MainLoop::Get()->RemoveListener(_drawTickListener);

		::ImGui::DestroyContext();

		CocoTrace("Destroyed ImGuiService")
	}

	void ImGuiService::HandleNewFrameTick(const TickInfo& tickInfo)
	{
		using namespace Coco::Input;

		if (!_inputLayer.IsValid() && Engine::Get()->GetServiceManager().HasService<InputService>())
		{
			_inputLayer = CreateManagedRef<ImGuiInputLayer>();
			Engine::Get()->GetServiceManager().GetService<InputService>().RegisterInputLayer(_inputLayer);

			CocoTrace("Registered ImGuiInputLayer")
		}

		if (!_platform->NewFrame(tickInfo))
			return;

		::ImGui::NewFrame();
		::ImGuizmo::BeginFrame();
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

			_platform->RenderViewport(viewport);
		}
	}
}