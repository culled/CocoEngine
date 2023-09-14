#include "SandboxApp.h"
#include <Coco/Core/Engine.h>
#include <Coco/Windowing/WindowService.h>
#include <Coco/Input/InputService.h>
#include <Coco/Rendering/RenderService.h>
#include <Coco/Rendering/Graphics/RHI/Vulkan/VulkanGraphicsPlatformFactory.h>

#include "Rendering/BasicRenderPass.h"

using namespace Coco;

MainApplication(SandboxApp)

SandboxApp::SandboxApp() : 
	Application(ApplicationCreateParameters("Sandbox", Version(0, 0, 1))),
	_tickListener(this, &SandboxApp::Tick, 0)
{
	Engine::Get()->GetMainLoop()->AddListener(_tickListener);
	Engine::Get()->GetMainLoop()->SetTargetTicksPerSecond(60);

	ServiceManager* services = Engine::Get()->GetServiceManager();
	{
		using namespace Coco::Input;
		services->Create<InputService>();
	}

	 {
		using namespace Coco::Rendering;
		GraphicsDeviceCreateParams deviceParams{};
		GraphicsPlatformCreateParams platformParams(*this, true);
		platformParams.DeviceCreateParameters = deviceParams;

		Vulkan::VulkanGraphicsPlatformFactory vulkanFactory(platformParams);
		services->Create<RenderService>(vulkanFactory);
	}

	{
		using namespace Coco::Windowing;
		WindowService* windowing = services->Create<WindowService>(true);
		WindowCreateParams windowCreateParams("Sandbox", SizeInt(1280, 720));
		Window* win = windowing->CreateWindow(windowCreateParams);
		win->Show();
	}

	_pipeline = CreateSharedRef<Rendering::RenderPipeline>();
	_pipeline->AddRenderPass(CreateSharedRef<BasicRenderPass>(), { 0 });

	LogTrace(_log, "Sandbox app initialized")
}

SandboxApp::~SandboxApp()
{
	LogTrace(_log, "Sandbox app shutdown")
}

void SandboxApp::Start()
{}

void SandboxApp::Tick(const TickInfo & tickInfo)
{
	ServiceManager* services = Engine::Get()->GetServiceManager();
	Input::InputService* input = services->Get<Input::InputService>();

	if (input->GetKeyboard()->WasKeyJustPressed(Input::KeyboardKey::Escape))
	{
		Quit();
		return;
	}

	Windowing::WindowService* windowing = services->Get<Windowing::WindowService>();
	Rendering::RenderService* rendering = services->Get<Rendering::RenderService>();
	
	std::vector<Windowing::Window*> visibleWindows = windowing->GetVisibleWindows();

	for (Windowing::Window* window : visibleWindows)
	{
		Rendering::GraphicsPresenter* presenter = window->GetPresenter();
		rendering->Render(*presenter, *_pipeline);
	}
}