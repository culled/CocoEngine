#include "EditorApplication.h"

#include <Coco/Input/InputService.h>
#include <Coco/Rendering/RenderService.h>
#include <Coco/Rendering/Graphics/RHI/Vulkan/VulkanGraphicsPlatformFactory.h>
#include <Coco/ImGui/ImGuiService.h>
#include <Coco/Windowing/WindowService.h>
#include <Coco/Core/Engine.h>

#include <imgui.h>

// TEMPORARY
#include <Coco/Rendering/Resources/BuiltInPipeline.h>
#include <Coco/Rendering/Mesh.h>
#include <Coco/Rendering/Material.h>
#include <Coco/Rendering/MeshUtilities.h>
#include <Coco/ECS/Components/Transform3DComponent.h>
#include <Coco/ECS/Components/Rendering/MeshRendererComponent.h>
#include <Coco/Rendering/Resources/BuiltinShaders.h>
// TEMPORARY

using namespace Coco::ECS;

namespace Coco
{
	MainApplication(EditorApplication)

	EditorApplication::EditorApplication() :
		Application(ApplicationCreateParameters("Coco Editor", Version(0, 0, 1))),
		_selection(),
		_mainWindow(),
		_updateTickListener(CreateManagedRef<TickListener>(this, &EditorApplication::HandleUpdateTick, 0)),
		_renderTickListener(CreateManagedRef<TickListener>(this, &EditorApplication::HandleRenderTick, 99)),
		_pipeline(BuiltInPipeline::Create(true)),
		_mainScene(Scene::Create()),
		_viewportClosedHandler(this, &EditorApplication::OnViewportPanelClosed)
	{
		SetupServices();
		CreateMainWindow();
		CreateMainScene();
		SetupDefaultLayout();

		MainLoop& loop = Engine::Get()->GetMainLoop();
		loop.AddListener(_updateTickListener);
		loop.AddListener(_renderTickListener);
	}

	EditorApplication::~EditorApplication()
	{
		MainLoop& loop = Engine::Get()->GetMainLoop();
		loop.RemoveListener(_updateTickListener);
		loop.RemoveListener(_renderTickListener);

		if (_viewport)
			CloseViewportPanel();

		_scenePanel.reset();
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
		_scenePanel = CreateUniqueRef<SceneHierarchyPanel>(_mainScene);
		_inspectorPanel = CreateUniqueRef<InspectorPanel>();
	}

	void EditorApplication::CreateMainScene()
	{
		using namespace Coco::Rendering;
		using namespace Coco::ECS;

		VertexDataFormat format{};
		format.HasUV0 = true;

		std::vector<VertexData> vertices;
		std::vector<uint32> indices;
		MeshUtilities::CreateXYGrid(Vector2::One, Vector3::Zero, format, vertices, indices);

		ResourceLibrary& resourceLibrary = Engine::Get()->GetResourceLibrary();

		SharedRef<Mesh> mesh = resourceLibrary.Create<Mesh>("Mesh");
		mesh->SetVertices(format, vertices);
		mesh->SetIndices(indices, 0);
		mesh->Apply();

		GraphicsPipelineState pipelineState{};

		SharedRef<Shader> shader = resourceLibrary.Create<Shader>("UnlitShader", "");
		shader->AddVariant(BuiltInShaders::UnlitVariant);

		SharedRef<Material> material = resourceLibrary.Create<Material>("UnlitMaterial", shader);
		material->SetFloat4("TintColor", Color::Red);

		_entity = _mainScene->CreateEntity("Test");
		_entity.AddComponent<Transform3DComponent>(Vector3::Forward, Quaternion::Identity, Vector3::One);

		std::unordered_map<uint32, SharedRef<MaterialDataProvider>> materials = { { 0, material } };
		_entity.AddComponent<MeshRendererComponent>(mesh, materials);

		_entity2 = _mainScene->CreateEntity("Test 2");
		_entity2.AddComponent<Transform3DComponent>(Vector3(2.0, 2.0, 2.0), Quaternion(Vector3::Up, Math::DegToRad(180.0)), Vector3::One * 2.0);
		_entity2.AddComponent<MeshRendererComponent>(mesh, materials);
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

			//if (ImGui::BeginMenu("View"))
			//{
			//	ShowViewMenu();
			//	ImGui::EndMenu();
			//}

			ImGui::EndMenuBar();
		}

		_viewport->Update(tickInfo);
		_inspectorPanel->Update(tickInfo);
		_scenePanel->Update(tickInfo);

		ImGui::End();
	}

	void EditorApplication::HandleRenderTick(const TickInfo& tickInfo)
	{
		if (!_mainWindow->IsVisible() || !_viewport)
			return;

		_viewport->Render(*_pipeline);
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
}

#ifdef COCO_PLATFORM_WINDOWS
#include <Coco/Platforms/Win32/EntryPoint.h>
#endif