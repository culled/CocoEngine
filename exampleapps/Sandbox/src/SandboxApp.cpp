#include "SandboxApp.h"
#include <Coco/Core/Engine.h>
#include <Coco/Windowing/WindowService.h>
#include <Coco/Input/InputService.h>
#include <Coco/Rendering/RenderService.h>
#include <Coco/Rendering/Graphics/RHI/Vulkan/VulkanGraphicsPlatformFactory.h>

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
		GraphicsPlatformCreateParams platformParams(*this);
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

		_focusChangedHandler.SetCallback([&](bool focused) { LogInfo(_log, "New focus state: {}", focused) return true; });
		_focusChangedHandler.Connect(win->OnFocusChanged);

		_positionChangedHandler.SetCallback([&](const Vector2Int& pos) { LogInfo(_log, "New position: {}", pos.ToString()) return true; });
		_positionChangedHandler.Connect(win->OnPositionChanged);

		_sizeChangedHandler.SetCallback([&](const SizeInt& size) { LogInfo(_log, "New size: {}", size.ToString()) return true; });
		_sizeChangedHandler.Connect(win->OnResized);

		_dpiChangedHandler.SetCallback([&](uint16 dpi) { LogInfo(_log, "New dpi: {}", dpi) return true; });
		_dpiChangedHandler.Connect(win->OnDPIChanged);
	}

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
	Input::InputService* input = Engine::Get()->GetServiceManager()->Get<Input::InputService>();

	if (input->GetKeyboard()->WasKeyJustPressed(Input::KeyboardKey::Escape))
	{
		Quit();
		return;
	}
}