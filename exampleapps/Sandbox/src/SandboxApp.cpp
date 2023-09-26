#include "SandboxApp.h"
#include <Coco/Core/Engine.h>
#include <Coco/Windowing/WindowService.h>
#include <Coco/Input/InputService.h>
#include <Coco/Rendering/RenderService.h>
#include <Coco/Rendering/Graphics/RHI/Vulkan/VulkanGraphicsPlatformFactory.h>

#include <Coco/ImGui/ImGuiService.h>

#include "Rendering/RenderPass3D.h"
#include "Rendering/RenderPass2D.h"

using namespace Coco;

MainApplication(SandboxApp)

SandboxApp::SandboxApp() : 
	Application(ApplicationCreateParameters("Sandbox", Version(0, 0, 1))),
	_tickListener(this, &SandboxApp::Tick, 0),
	_attachmentCache(CreateUniqueRef<AttachmentCache>())
{
	MainLoop::Get()->AddListener(_tickListener);
	//MainLoop::Get()->SetTargetTicksPerSecond(144);

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
		services->CreateService<RenderService>(vulkanFactory);
	}

	{
		using namespace Coco::Windowing;
		WindowService& windowing = services->CreateService<WindowService>(true);
		WindowCreateParams windowCreateParams("Sandbox", SizeInt(1280, 720));
		Ref<Window> win = windowing.CreateWindow(windowCreateParams);
		win->GetPresenter()->SetVSync(Rendering::VSyncMode::Immediate);
		//win->GetPresenter()->SetMaximumFramesInFlight(2);
		win->Show();
	}

	{
		using namespace Coco::ImGuiCoco;
		services->CreateService<ImGuiService>(true);
	}

	_pipeline3D = CreateSharedRef<Rendering::RenderPipeline>();

	{
		std::array<uint8, 2> bindings = { 0, 1 };
		SharedRef<RenderPass3D> pass = CreateSharedRef<RenderPass3D>();
		_pipeline3D->AddRenderPass(pass, bindings);
	}

	_renderViewProvider3D = CreateUniqueRef<RenderViewProvider3D>(*_attachmentCache);
	_sceneDataProvider3D = CreateUniqueRef<SceneDataProvider3D>();

	_pipeline2D = CreateSharedRef<Rendering::RenderPipeline>();

	{
		std::array<uint8, 1> bindings = { 0 };
		SharedRef<RenderPass2D> pass = CreateSharedRef<RenderPass2D>();
		_pipeline2D->AddRenderPass(pass, bindings);
	}

	_renderViewProvider2D = CreateUniqueRef<RenderViewProvider2D>(*_attachmentCache);
	_sceneDataProvider2D = CreateUniqueRef<SceneDataProvider2D>();

	_imGuiLayer = CreateUniqueRef<ImGuiLayer>(*_renderViewProvider3D);

	LogTrace(_log, "Sandbox app initialized")
}

SandboxApp::~SandboxApp()
{
	_pipeline3D.reset();
	_renderViewProvider3D.reset();
	_sceneDataProvider3D.reset();

	_pipeline2D.reset();
	_renderViewProvider2D.reset();
	_sceneDataProvider2D.reset();

	_attachmentCache.reset();
	_imGuiLayer.reset();

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
		std::array<SceneDataProvider*, 1> dataProviders = { _sceneDataProvider3D.get()};

		rendering.Render(mainWindow->GetPresenter(), *_pipeline3D, *_renderViewProvider3D, dataProviders);

		dataProviders = { _sceneDataProvider2D.get() };
		rendering.Render(mainWindow->GetPresenter(), *_pipeline2D, *_renderViewProvider2D, dataProviders);

		_imGuiLayer->DrawPostRender();
	}
}