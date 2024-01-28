#include "SandboxApp.h"
#include <Coco/Core/Engine.h>

#include <Coco/Input/InputService.h>
#include <Coco/Rendering/RenderService.h>
#include <Coco/Rendering/Graphics/RHI/Vulkan/VulkanGraphicsPlatformFactory.h>
#include <Coco/Rendering/BuiltIn/BuiltInShaderRenderPass.h>

#include <Coco/Windowing/WindowService.h>

#include <Coco/ImGui/ImGuiService.h>

#include <Coco/ECS/ECSService.h>
#include <Coco/ECS/Systems/Rendering/CameraSystem.h>

#include "Rendering/TestRenderPass.h"

#include <imgui.h>

using namespace Coco;

MainApplication(SandboxApp)

SandboxApp::SandboxApp() : 
	Application(ApplicationCreateParameters("Sandbox", Version(0, 0, 1), LogMessageSeverity::Trace)),
	_tickListener(CreateManagedRef<TickListener>(this, &SandboxApp::Tick, 0)),
	_renderTickListener(CreateManagedRef<TickListener>(this, &SandboxApp::RenderTick, 10))
{
	Engine::Get()->GetLog().SetMinimumSeverity(LogMessageSeverity::Trace);

	MainLoop& loop = *MainLoop::Get();
	loop.AddTickListener(_tickListener);
	loop.AddTickListener(_renderTickListener);
	//loop.SetTargetTicksPerSecond(144);

	InitializeServices();
	InitializeResources();

	LogInfo(_log, "Sandbox app initialized")
}

SandboxApp::~SandboxApp()
{
	LogInfo(_log, "Sandbox app shutdown")
}

void SandboxApp::Start()
{
	using namespace Coco::Windowing;

	WindowCreateParams windowCreateParams("Sandbox", SizeInt(1280, 720));
	Ref<Window> win = WindowService::Get()->CreateWindow(windowCreateParams);
	win->GetPresenter()->SetVSyncMode(Rendering::VSyncMode::EveryVBlank);
	win->Show();
}

void SandboxApp::InitializeServices()
{
	ServiceManager* services = ServiceManager::Get();

	{
		using namespace Coco::Input;
		services->CreateService<InputService>();
	}

	{
		using namespace Coco::Rendering;

		Vulkan::VulkanGraphicsPlatformCreateParams createParams(*this, true);
#ifdef _DEBUG
		createParams.UseValidationLayers = true;
#endif

		Vulkan::VulkanGraphicsPlatformFactory vulkanFactory(createParams);
		services->CreateService<RenderService>(vulkanFactory);
	}

	{
		using namespace Coco::Windowing;
		services->CreateService<WindowService>();
	}
	
	{
		using namespace Coco::ImGuiCoco;
		services->CreateService<ImGuiService>(true, false);
	}
	
	{
		using namespace Coco::ECS;
		services->CreateService<ECSService>();
	}
}

void SandboxApp::InitializeResources()
{
	ResourceLibrary& resources = Engine::Get()->GetResourceLibrary();

	_renderViewProvider3D = CreateSharedRef<Rendering::BuiltInRenderViewDataProvider>(
		Color(0.1, 0.2, 0.3, 1.0),
		Vector2(1.0, 0.0),
		Rendering::MSAASamples::One,
		Rendering::ShaderUniformLayout(
			{
				Rendering::ShaderUniform("TestGlobal", Rendering::ShaderUniformType::Float, Rendering::ShaderStageFlags::Fragment, 0.f)
			}
	));
	
	_pipeline3D = resources.Create<Rendering::RenderPipeline>(ResourceID("SandboxApp::RenderPipeline"), Rendering::AttachmentOptionsFlags::Clear);
	std::array<uint32, 1> bindings = { 0 };
	
	_pipeline3D->AddRenderPass(
		CreateSharedRef<TestRenderPass>(),
		bindings);

	_pipelineECS = resources.Create<Rendering::RenderPipeline>(ResourceID("SandboxApp::RenderPipelineECS"), Rendering::AttachmentOptionsFlags::Clear);

	std::array<uint32, 2> ecsBindings = { 0, 1 };
	_pipelineECS->AddRenderPass(
		CreateSharedRef<Rendering::BuiltInShaderRenderPass>(false),
		ecsBindings);

	_sceneProvider3D = CreateUniqueRef<SceneProvider3D>();

	_ecsScene = CreateUniqueRef<ECSScene>();
}

void SandboxApp::Tick(const TickInfo& tickInfo)
{
	ServiceManager* services = ServiceManager::Get();
	Input::InputService& input = services->GetService<Input::InputService>();

	if (input.GetKeyboard().WasKeyJustPressed(Input::KeyboardKey::Escape))
	{
		Quit();
		return;
	}

	_ecsScene->Update(tickInfo);

	ImGui::ShowDemoWindow();

	/*_imGuiLayer->Draw();

	Windowing::WindowService& windowing = services->GetService<Windowing::WindowService>();
	Rendering::RenderService& rendering = services->GetService<Rendering::RenderService>();
	
	Ref<Windowing::Window> mainWindow = windowing.GetMainWindow();

	if(mainWindow->IsVisible())
	{
		ECS::CameraSystem::Render(_cameraEntity, mainWindow->GetPresenter(), *_pipeline3D, RenderViewProvider3D::sGlobalLayout);

		std::array<SceneDataProvider*, 1> dataProviders = { _sceneDataProvider2D.get()};

		rendering.Render(mainWindow->GetPresenter(), *_pipeline2D, *_renderViewProvider2D, dataProviders);
	}*/
}

void SandboxApp::RenderTick(const TickInfo& tickInfo)
{
	ServiceManager* services = ServiceManager::Get();

	Windowing::WindowService& windowing = services->GetService<Windowing::WindowService>();
	Rendering::RenderService& rendering = services->GetService<Rendering::RenderService>();

	Ref<Windowing::Window> mainWindow = windowing.GetMainWindow();

	if (mainWindow->IsVisible())
	{
		//ECS::Entity cameraEntity = _ecsScene->GetCameraEntity();
		//cameraEntity.GetComponent<ECS::CameraComponent>().Render(0, mainWindow->GetPresenter(), *_pipelineECS);
		ECS::CameraSystem::RenderScene(0, _ecsScene->GetScene(), mainWindow->GetPresenter(), *_pipelineECS);

		//std::array<Rendering::SceneDataProvider*, 1> dataProviders = { _sceneProvider3D.get() };
		//rendering.Render(0, mainWindow->GetPresenter(), *_pipeline3D, *_renderViewProvider3D, dataProviders);
	}
}
