#include "EditorApplication.h"

#include <Coco/Input/InputService.h>
#include <Coco/Rendering/RenderService.h>
#include <Coco/Rendering/Graphics/RHI/Vulkan/VulkanGraphicsPlatformFactory.h>
#include <Coco/ImGui/ImGuiService.h>
#include <Coco/Windowing/WindowService.h>
#include <Coco/Core/Engine.h>

#include <imgui.h>

// TEMPORARY
//#include "Rendering/PickingRenderPass.h"
#include <Coco/Rendering/Pipeline/RenderPasses/BasicShaderRenderPass.h>
#include <Coco/Rendering/BuiltIn/BuiltInShaderRenderPass.h>
// TEMPORARY

using namespace Coco::ECS;
using namespace Coco::Rendering;
using namespace Coco::ImGuiCoco;
using namespace Coco::Windowing;

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
		_renderTickListener(CreateManagedRef<TickListener>(this, &EditorApplication::HandleRenderTick, 99))
	{
		SetupServices();
		CreateMainWindow();
		CreateResources();
		CreateMainScene();

		Input::InputService::Get()->RegisterInputLayer(_inputLayer);

		MainLoop& loop = Engine::Get()->GetMainLoop();
		loop.AddTickListener(_updateTickListener);
		loop.AddTickListener(_renderTickListener);

		MainLoop::Get()->SetTimeScale(0.0);
	}

	EditorApplication::~EditorApplication()
	{
		MainLoop& loop = Engine::Get()->GetMainLoop();
		loop.RemoveTickListener(_updateTickListener);
		loop.RemoveTickListener(_renderTickListener);

		Input::InputService::Get()->UnregisterInputLayer(_inputLayer);

		_mainWindow.Invalidate();
		_fontData.clear();
	}

	void EditorApplication::SetupServices()
	{
		ServiceManager& services = Engine::Get()->GetServiceManager();

		InputService& input = services.CreateService<InputService>();

		Vulkan::VulkanGraphicsPlatformCreateParams platformParams(*this, true);
		Vulkan::VulkanGraphicsPlatformFactory vulkanFactory(platformParams);
		services.CreateService<RenderService>(vulkanFactory);

		services.CreateService<ImGuiService>(true, true);

		services.CreateService<WindowService>(true);
	}

	void EditorApplication::CreateMainWindow()
	{
		WindowCreateParams createParams("Coco Editor", SizeInt(1440, 810));
		createParams.CanResize = true;
		createParams.InitialState = WindowState::Maximized;
		
		_mainWindow = WindowService::Get()->CreateWindow(createParams);

		Assert(_mainWindow.IsValid())

		_mainWindow->Show();
	}

	void EditorApplication::CreateResources()
	{
		ResourceLibrary& resources = Engine::Get()->GetResourceLibrary();

		_renderPipeline = resources.Create<RenderPipeline>(ResourceID("Editor::RenderPipeline"));

		std::array<uint32, 2> bindings = { 0, 1 };
		_renderPipeline->AddRenderPass(CreateSharedRef<BuiltInShaderRenderPass>(true), bindings);

		File f = Engine::Get()->GetFileSystem().OpenFile("ui/fonts/Roboto/Roboto-Regular.ttf", FileOpenFlags::Read);
		_fontData = f.ReadToEnd();
		f.Close();

		ImGuiIO& io = ImGui::GetIO();
		ImFontConfig fontConfig;
		fontConfig.FontDataOwnedByAtlas = false;
		io.FontDefault = io.Fonts->AddFontFromMemoryTTF(_fontData.data(), static_cast<int>(_fontData.size()), 16.f, &fontConfig);
		ImGuiService::Get()->GetPlatform().RebuildFontTexture();
	}

	void EditorApplication::CreateMainScene()
	{
		ResourceLibrary& resourceLibrary = Engine::Get()->GetResourceLibrary();

		_mainScene = resourceLibrary.Create<Scene>(ResourceID("Scene"));
	}

	void EditorApplication::HandleUpdateTick(const TickInfo& tickInfo)
	{
		DrawUI();
	}

	void EditorApplication::HandleRenderTick(const TickInfo& tickInfo)
	{
		if (!_mainWindow->IsVisible())
			return;
	}

	void EditorApplication::DrawUI()
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

		ImGui::End();
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

		ImGui::Separator();

		if (ImGui::MenuItem("Exit"))
		{
			Quit();
		}
	}

	void EditorApplication::ShowViewMenu()
	{
		// TODO
	}

	void EditorApplication::NewScene()
	{
		SharedRef<Scene> scene = Engine::Get()->GetResourceLibrary().Create<Scene>(ResourceID("Scene"));
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
		//SharedRef<Scene> newScene = Engine::Get()->GetResourceLibrary().GetOrLoad<Scene>(scenePath, true);
		//ChangeScenes(newScene);
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

		//auto& resources = engine->GetResourceLibrary();
		//resources.Save(path, _mainScene, true);
		//resources.SaveAll();
	}

	void EditorApplication::StartPlayInEditor()
	{
		MainLoop& mainLoop = *MainLoop::Get();
		mainLoop.SetTimeScale(1.0);
		mainLoop.SetCurrentTickTime(0.0);

		// TODO: duplicate scene
		_mainScene->SetSimulateMode(SceneSimulateMode::Running);

		CocoInfo("Started Play In Editor")
	}

	void EditorApplication::StopPlayInEditor()
	{
		MainLoop::Get()->SetTimeScale(0.0);
		_mainScene->SetSimulateMode(SceneSimulateMode::Stopped);

		// TODO: restore scene;

		CocoInfo("Stopped Play In Editor")
	}

	void EditorApplication::ChangeScenes(SharedRef<Scene> newScene)
	{
		if (_mainScene->GetSimulateMode() == SceneSimulateMode::Running)
			_mainScene->SetSimulateMode(SceneSimulateMode::Stopped);

		_selection.ClearSelectedEntity();
		_mainScene = newScene;
	}
}

#ifdef COCO_PLATFORM_WINDOWS
#include <Coco/Platforms/Win32/EntryPoint.h>
#endif