#include "EditorApplication.h"

#include <Coco/Input/InputService.h>
#include <Coco/Rendering/RenderService.h>
#include <Coco/Rendering/Graphics/RHI/Vulkan/VulkanGraphicsPlatformFactory.h>
#include <Coco/ImGui/ImGuiService.h>
#include <Coco/Windowing/WindowService.h>
#include "UI/ComponentUI.h"
#include <Coco/Core/Engine.h>

#include <imgui.h>

// TEMPORARY
#include <Coco/Rendering/Resources/BuiltInPipeline.h>
#include "Rendering/PickingRenderPass.h"
#include <Coco/Rendering/Pipeline/RenderPasses/BasicShaderRenderPass.h>
#include <Coco/Rendering/Pipeline/RenderPasses/BuiltInRenderPass.h>
#include <Coco/ECS/Serializers/SceneSerializer.h>
// TEMPORARY

using namespace Coco::ECS;

// Current framebuffer layout:
// 0 - main color
// 1 - depth
// 2 - frame picking

namespace Coco
{
	MainApplication(EditorApplication)

	EditorApplication::EditorApplication() :
		Application(ApplicationCreateParameters("Coco Editor", Version(0, 0, 1))),
		_selection(),
		_inputLayer(CreateManagedRef<EditorInputLayer>()),
		_mainWindow(),
		_updateTickListener(CreateManagedRef<TickListener>(this, &EditorApplication::HandleUpdateTick, 0)),
		_renderTickListener(CreateManagedRef<TickListener>(this, &EditorApplication::HandleRenderTick, 99)),
		_pipeline(BuiltInPipeline::Create(true, true)),
		_viewportClosedHandler(this, &EditorApplication::OnViewportPanelClosed),
		_fileDoubleClickedHandler(this, &EditorApplication::OnFileDoubleClicked)
	{
		Engine::Get()->GetResourceLibrary().CreateSerializer<SceneSerializer>();

		SetupServices();
		CreateMainWindow();
		CreateMainScene();
		SetupDefaultLayout();

		ComponentUI::RegisterBuiltInComponentUIs();

		MainLoop& loop = Engine::Get()->GetMainLoop();
		loop.AddListener(_updateTickListener);
		loop.AddListener(_renderTickListener);

		std::array<uint8, 2> bindings = { 2, 1 };
		_pipeline->AddRenderPass(CreateSharedRef<PickingRenderPass>(), bindings);
	}

	EditorApplication::~EditorApplication()
	{
		MainLoop& loop = Engine::Get()->GetMainLoop();
		loop.RemoveListener(_updateTickListener);
		loop.RemoveListener(_renderTickListener);

		Input::InputService::Get()->UnregisterInputLayer(_inputLayer);

		if (_viewport)
			CloseViewportPanel();

		_mainWindow.Invalidate();
		_gameViewport.reset();
		_scenePanel.reset();
		_inspectorPanel.reset();
		_contentPanel.reset();
		_fontData.clear();
	}

	void EditorApplication::SetupServices()
	{
		using namespace Coco::Input;
		using namespace Coco::Rendering;
		using namespace Coco::ImGuiCoco;
		using namespace Coco::Windowing;

		ServiceManager& services = Engine::Get()->GetServiceManager();

		InputService& input = services.CreateService<InputService>();
		input.RegisterInputLayer(_inputLayer);

		GraphicsDeviceCreateParams deviceParams{};
		GraphicsPlatformCreateParams platformParams(*this, true);
		platformParams.DeviceCreateParameters = deviceParams;

		Vulkan::VulkanGraphicsPlatformFactory vulkanFactory(platformParams, Vulkan::VulkanGraphicsPlatformFactory::sDefaultAPIVersion, true);
		services.CreateService<RenderService>(vulkanFactory, true);

		services.CreateService<ImGuiService>(true, true);

		services.CreateService<WindowService>(true);
	}

	void EditorApplication::CreateMainWindow()
	{
		using namespace Coco::Windowing;
		WindowCreateParams createParams("Coco Editor", SizeInt(1440, 810));
		createParams.CanResize = true;
		createParams.InitialState = WindowState::Maximized;
		
		_mainWindow = WindowService::Get()->CreateWindow(createParams);

		Assert(_mainWindow.IsValid())

		_mainWindow->Show();
	}

	void EditorApplication::SetupDefaultLayout()
	{
		using namespace Coco::ImGuiCoco;

		File f = Engine::Get()->GetFileSystem().OpenFile("ui/fonts/Roboto/Roboto-Regular.ttf", FileOpenFlags::Read);
		_fontData = f.ReadToEnd();
		f.Close();

		ImGuiIO& io = ImGui::GetIO();
		ImFontConfig fontConfig;
		fontConfig.FontDataOwnedByAtlas = false;
		io.FontDefault = io.Fonts->AddFontFromMemoryTTF(_fontData.data(), static_cast<int>(_fontData.size()), 16.f, &fontConfig);
		ImGuiService::Get()->GetPlatform().RebuildFontTexture();

		_viewport = CreateViewportPanel();
		_scenePanel = CreateUniqueRef<SceneHierarchyPanel>(_mainScene);
		_inspectorPanel = CreateUniqueRef<InspectorPanel>();
		_contentPanel = CreateUniqueRef<ContentPanel>();
		_fileDoubleClickedHandler.Connect(_contentPanel->OnFileDoubleClicked);
		_gameViewport = CreateUniqueRef<GamePanel>(_mainScene);
	}

	void EditorApplication::CreateMainScene()
	{
		using namespace Coco::Rendering;
		using namespace Coco::ECS;

		ResourceLibrary& resourceLibrary = Engine::Get()->GetResourceLibrary();

		_mainScene = resourceLibrary.Create<Scene>("Scene");
		_mainScene->UseDefaultSystems();
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

		ImGuiStyle& style = ImGui::GetStyle();

		float minWidth = style.WindowMinSize.x;
		style.WindowMinSize.x = 300.f;

		ImGuiID dockspace_id = ImGui::GetID("CocoEditorDockspace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

		style.WindowMinSize.x = minWidth;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				ShowFileMenu();
				ImGui::EndMenu();
			}

			//if (ImGui::BeginMenu("View"))
			//{
			//	ShowViewMenu();
			//	ImGui::EndMenu();
			//}

			ImGui::EndMenuBar();
		}

		_inspectorPanel->Update(tickInfo);
		_scenePanel->Update(tickInfo);
		_viewport->Update(tickInfo);
		_contentPanel->Update(tickInfo);
		_gameViewport->Update(tickInfo);

		ImGui::End();
	}

	void EditorApplication::HandleRenderTick(const TickInfo& tickInfo)
	{
		if (!_mainWindow->IsVisible() || !_viewport)
			return;

		_viewport->Render(*_pipeline);
		_gameViewport->Render(*_pipeline);
	}

	void EditorApplication::ShowFileMenu()
	{
		if (ImGui::MenuItem("New Scene", "Ctrl+N"))
		{
			NewScene();
		}

		if (ImGui::MenuItem("Open Scene...", "Ctrl+O"))
		{
			OpenScene();
		}

		if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
		{
			SaveSceneAs();
		}

		if (ImGui::MenuItem("Save All Resources"))
		{
			Engine::Get()->GetResourceLibrary().SaveAll();
		}

		ImGui::Separator();

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
		UniqueRef<ViewportPanel> viewport = CreateUniqueRef<ViewportPanel>("Viewport", _mainScene);
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

	void EditorApplication::NewScene()
	{
		SharedRef<Scene> scene = Engine::Get()->GetResourceLibrary().Create<Scene>("Scene");
		ChangeScenes(scene);
	}

	void EditorApplication::OpenScene()
	{
		Engine* engine = Engine::Get();
		string path = engine->GetPlatform().ShowOpenFileDialog(
			{ 
				{"Coco Scene (*.cscene)", "*.cscene" } 
			}
		);

		if (path.empty())
			return;

		OpenScene(path);
	}

	void EditorApplication::OpenScene(const FilePath& scenePath)
	{
		SharedRef<Scene> newScene = Engine::Get()->GetResourceLibrary().GetOrLoad<Scene>(scenePath, true);
		ChangeScenes(newScene);
	}

	void EditorApplication::SaveSceneAs()
	{
		Engine* engine = Engine::Get();
		string path = engine->GetPlatform().ShowSaveFileDialog(
			{
				{"Coco Scene (*.cscene)", "*.cscene" }
			}
		);

		if (path.empty())
			return;

		auto& resources = engine->GetResourceLibrary();
		resources.Save(path, _mainScene, true);
		resources.SaveAll();
	}

	void EditorApplication::ChangeScenes(SharedRef<Scene> newScene)
	{
		_selection.ClearSelectedEntity();
		_mainScene = newScene;

		_viewport->SetCurrentScene(_mainScene);
		_scenePanel->SetCurrentScene(_mainScene);
		_gameViewport->SetCurrentScene(_mainScene);
	}

	bool EditorApplication::OnFileDoubleClicked(const FilePath& file)
	{
		if (file.GetExtension() == ".cscene")
		{
			OpenScene(file);
			return true;
		}

		return false;
	}
}

#ifdef COCO_PLATFORM_WINDOWS
#include <Coco/Platforms/Win32/EntryPoint.h>
#endif