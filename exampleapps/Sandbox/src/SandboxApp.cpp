#include "SandboxApp.h"
#include <Coco/Core/Engine.h>
#include <Coco/Windowing/WindowService.h>
#include <Coco/Input/InputService.h>
#include <Coco/Rendering/RenderService.h>
#include <Coco/Rendering/Graphics/RHI/Vulkan/VulkanGraphicsPlatformFactory.h>
#include <Coco/Rendering/Gizmos/GizmoRenderPass.h>
#include <Coco/ECS/Components/Transform3DComponent.h>
#include <Coco/ECS/Components/Rendering/CameraComponent.h>
#include <Coco/ECS/Components/Rendering/CameraSystem.h>
#include <Coco/ECS/Components/NativeScriptComponent.h>

#include <Coco/ImGui/ImGuiService.h>

#include "Rendering/RenderPass3D.h"
#include "Rendering/RenderPass2D.h"
#include "Rendering/RenderViewProvider3D.h"
#include "Rendering/SceneDataProvider3D.h"
#include "Scripts/CameraController.h"

using namespace Coco;

MainApplication(SandboxApp)

SandboxApp::SandboxApp() : 
	Application(ApplicationCreateParameters("Sandbox", Version(0, 0, 1))),
	_tickListener(CreateManagedRef<TickListener>(this, &SandboxApp::Tick, 0))
{
	//Engine::Get()->GetLog().SetMinimumSeverity(LogMessageSeverity::Trace);

	MainLoop::Get()->AddListener(_tickListener);
	MainLoop::Get()->SetTargetTicksPerSecond(144);

	ServiceManager* services = ServiceManager::Get();
	{
		using namespace Coco::Input;
		services->CreateService<InputService>();
	}

	{
		using namespace Coco::Rendering;
		GraphicsDeviceCreateParams deviceParams{};

		GraphicsPlatformCreateParams platformParams(*this, true);
		platformParams.DeviceCreateParameters = deviceParams;

		Vulkan::VulkanGraphicsPlatformFactory vulkanFactory(platformParams, Vulkan::VulkanGraphicsPlatformFactory::sDefaultAPIVersion, true);
		services->CreateService<RenderService>(vulkanFactory, true);
	}

	{
		using namespace Coco::Windowing;
		WindowService& windowing = services->CreateService<WindowService>(true);
		WindowCreateParams windowCreateParams("Sandbox", SizeInt(1280, 720));
		Ref<Window> win = windowing.CreateWindow(windowCreateParams);
		//win->GetPresenter()->SetVSync(Rendering::VSyncMode::Immediate);
		win->Show();
	}

	{
		using namespace Coco::ImGuiCoco;
		services->CreateService<ImGuiService>(true, false);
	}

	ResourceLibrary& resources = Engine::Get()->GetResourceLibrary();

	{
		using namespace Coco::ECS;
		_scene = Scene::CreateWithDefaultSystems("Scene");

		_cameraEntity = _scene->CreateEntity("Camera");

		_cameraEntity.AddComponent<Transform3DComponent>();

		CameraComponent& camera = _cameraEntity.AddComponent<CameraComponent>();
		camera.SetPerspectiveProjection(Math::DegToRad(90.0), 0.1, 100.0);
		camera.SetClearColor(Color(0.1, 0.2, 0.3, 1.0));

		NativeScriptComponent& script = _cameraEntity.AddComponent<NativeScriptComponent>();
		script.Attach<CameraController>();
	}

	_pipeline3D = resources.Create<Rendering::RenderPipeline>("3D Pipeline", AttachmentOptionFlags::Clear);

	{
		std::array<uint8, 2> bindings = { 0, 1 };
		SharedRef<RenderPass3D> pass = CreateSharedRef<RenderPass3D>();
		_pipeline3D->AddRenderPass(pass, bindings);

		_pipeline3D->AddRenderPass(CreateSharedRef<GizmoRenderPass>(), bindings);
	}

	SceneDataProvider3D::SetupScene(_scene);

	_pipeline2D = resources.Create<Rendering::RenderPipeline>("2D Pipeline", AttachmentOptionFlags::Preserve);

	{
		std::array<uint8, 1> bindings = { 0 };
		SharedRef<RenderPass2D> pass = CreateSharedRef<RenderPass2D>();
		_pipeline2D->AddRenderPass(pass, bindings);
	}

	_renderViewProvider2D = CreateUniqueRef<RenderViewProvider2D>();
	_sceneDataProvider2D = CreateUniqueRef<SceneDataProvider2D>();

	_imGuiLayer = CreateUniqueRef<ImGuiLayer>(_cameraEntity);

	LogTrace(_log, "Sandbox app initialized")
}

SandboxApp::~SandboxApp()
{
	_imGuiLayer.reset();

	_pipeline3D.reset();

	_pipeline2D.reset();
	_renderViewProvider2D.reset();
	_sceneDataProvider2D.reset();

	LogTrace(_log, "Sandbox app shutdown")
}

void SandboxApp::Start()
{}

void SandboxApp::Tick(const TickInfo & tickInfo)
{
	ServiceManager* services = ServiceManager::Get();
	Input::InputService& input = services->GetService<Input::InputService>();

	if (input.GetKeyboard().WasKeyJustPressed(Input::KeyboardKey::Escape))
	{
		Quit();
		return;
	}

	_imGuiLayer->Draw();

	Windowing::WindowService& windowing = services->GetService<Windowing::WindowService>();
	Rendering::RenderService& rendering = services->GetService<Rendering::RenderService>();
	
	Ref<Windowing::Window> mainWindow = windowing.GetMainWindow();

	if(mainWindow->IsVisible())
	{
		ECS::CameraSystem::Render(_cameraEntity, mainWindow->GetPresenter(), *_pipeline3D, RenderViewProvider3D::sGlobalLayout);

		std::array<SceneDataProvider*, 1> dataProviders = { _sceneDataProvider2D.get()};

		rendering.Render(mainWindow->GetPresenter(), *_pipeline2D, *_renderViewProvider2D, dataProviders);
	}
}