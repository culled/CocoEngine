#include "EditorApplication.h"

#include <Coco/Input/InputService.h>
#include <Coco/Rendering/RenderService.h>
#include <Coco/Rendering/Graphics/RHI/Vulkan/VulkanGraphicsPlatformFactory.h>
#include <Coco/ImGui/ImGuiService.h>
#include <Coco/Windowing/WindowService.h>
#include <Coco/Core/Engine.h>

#include <imgui.h>

// TEMPORARY
#include "TestRenderPass.h"
// TEMPORARY

namespace Coco
{
	MainApplication(EditorApplication)

	EditorApplication::EditorApplication() :
		Application(ApplicationCreateParameters("Coco Editor", Version(0, 0, 1))),
		_mainWindow(),
		_updateTickListener(CreateManagedRef<TickListener>(this, &EditorApplication::HandleUpdateTick, 0)),
		_renderTickListener(CreateManagedRef<TickListener>(this, &EditorApplication::HandleRenderTick, 99)),
		_renderTest(CreateUniqueRef<RenderTest>()),
		_pipeline(CreateUniqueRef<RenderPipeline>()),
		_viewportClosedHandler(this, &EditorApplication::OnViewportPanelClosed)
	{
		SetupServices();
		CreateMainWindow();
		SetupDefaultLayout();

		MainLoop& loop = Engine::Get()->GetMainLoop();
		loop.AddListener(_updateTickListener);
		loop.AddListener(_renderTickListener);

		std::array<uint8, 1> bindings = { 0 };
		_pipeline->AddRenderPass(CreateSharedRef<TestRenderPass>(), bindings);
	}

	EditorApplication::~EditorApplication()
	{
		MainLoop& loop = Engine::Get()->GetMainLoop();
		loop.RemoveListener(_updateTickListener);
		loop.RemoveListener(_renderTickListener);

		_mainWindow.Invalidate();
	}

	void EditorApplication::SetupServices()
	{
		using namespace Coco::Input;
		using namespace Coco::Rendering;
		using namespace Coco::ImGuiCoco;
		using namespace Coco::Windowing;

		ServiceManager& services = Engine::Get()->GetServiceManager();

		services.CreateService<InputService>();

		GraphicsDeviceCreateParams deviceParams{};
		GraphicsPlatformCreateParams platformParams(*this, true);
		platformParams.DeviceCreateParameters = deviceParams;

		Vulkan::VulkanGraphicsPlatformFactory vulkanFactory(platformParams, Vulkan::VulkanGraphicsPlatformFactory::sDefaultAPIVersion, true);
		services.CreateService<RenderService>(vulkanFactory, true);

		services.CreateService<ImGuiService>(true);

		services.CreateService<WindowService>(true);
	}

	void EditorApplication::CreateMainWindow()
	{
		using namespace Coco::Windowing;
		WindowCreateParams createParams("Coco Editor", SizeInt(1280, 720));
		createParams.CanResize = true;
		
		_mainWindow = WindowService::Get()->CreateWindow(createParams);

		Assert(_mainWindow.IsValid())

		_mainWindow->Show();
	}

	void EditorApplication::SetupDefaultLayout()
	{
		_viewport = CreateViewportPanel();
	}

	void EditorApplication::HandleUpdateTick(const TickInfo& tickInfo)
	{
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		bool open = true;
		ImGui::Begin("DockSpace", &open, window_flags);
		ImGui::PopStyleVar(3);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiID dockspace_id = ImGui::GetID("CocoEditorDockspace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				ShowFileMenu();
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				ShowViewMenu();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::End();
	}

	void EditorApplication::HandleRenderTick(const TickInfo& tickInfo)
	{
		if (!_mainWindow->IsVisible() || !_viewport)
			return;

		std::array<SceneDataProvider*, 1> providers = { _renderTest.get() };
		_viewport->Render(*_pipeline, providers);
	}

	void EditorApplication::ShowFileMenu()
	{
		if (ImGui::MenuItem("Exit"))
		{
			Quit();
		}
	}

	void EditorApplication::ShowViewMenu()
	{
		bool viewport = _viewport.get() != nullptr;
		if (ImGui::Checkbox("Viewport", &viewport))
		{
			if (viewport)
			{
				_viewport = CreateViewportPanel();
			}
			else
			{
				CloseViewportPanel();
			}
		}
	}

	UniqueRef<ViewportPanel> EditorApplication::CreateViewportPanel()
	{
		UniqueRef<ViewportPanel> viewport = CreateUniqueRef<ViewportPanel>("Viewport");
		_viewportClosedHandler.Connect(viewport->OnClosed);

		return viewport;
	}

	void EditorApplication::CloseViewportPanel()
	{
		_viewportClosedHandler.Disconnect(_viewport->OnClosed);
		_viewport.reset();
	}

	bool EditorApplication::OnViewportPanelClosed(const ViewportPanel& panel)
	{
		CloseViewportPanel();

		return true;
	}
}

#ifdef COCO_PLATFORM_WINDOWS
#include <Coco/Platforms/Win32/EntryPoint.h>
#endif