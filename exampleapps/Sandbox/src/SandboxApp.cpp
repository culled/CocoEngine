#include "SandboxApp.h"
#include <Coco/Core/Engine.h>
#include <Coco/Windowing/WindowService.h>
#include <Coco/Input/InputService.h>
#include <Coco/Rendering/RenderService.h>
#include <Coco/Rendering/Graphics/RHI/Vulkan/VulkanGraphicsPlatformFactory.h>

#include <Coco/ImGui/ImGuiService.h>
#include <imgui.h>

#include "Rendering/BasicRenderPass.h"

using namespace Coco;

MainApplication(SandboxApp)

SandboxApp::SandboxApp() : 
	Application(ApplicationCreateParameters("Sandbox", Version(0, 0, 1))),
	_tickListener(this, &SandboxApp::Tick, 0)
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

		Vulkan::VulkanGraphicsPlatformFactory vulkanFactory(platformParams);
		services->CreateService<RenderService>(vulkanFactory);
	}

	{
		using namespace Coco::Windowing;
		WindowService& windowing = services->CreateService<WindowService>(true);
		WindowCreateParams windowCreateParams("Sandbox", SizeInt(1280, 720));
		Ref<Window> win = windowing.CreateWindow(windowCreateParams);
		//win->GetPresenter()->SetVSync(Rendering::VSyncMode::Immediate);
		win->Show();
	}

	_pipeline = CreateSharedRef<Rendering::RenderPipeline>();

	std::array<uint8, 2> bindings = { 0, 1 };
	SharedRef<BasicRenderPass> pass = CreateSharedRef<BasicRenderPass>();
	_pipeline->AddRenderPass(pass, bindings);

	_renderViewProvider = CreateUniqueRef<BasicRenderViewProvider>();
	_sceneDataProvider = CreateUniqueRef<BasicSceneDataProvider>();

	{
		using namespace Coco::ImGuiCoco;
		services->CreateService<ImGuiService>();
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
	ServiceManager* services = ServiceManager::Get();
	Input::InputService& input = services->GetService<Input::InputService>();

	if (input.GetKeyboard().WasKeyJustPressed(Input::KeyboardKey::Escape))
	{
		Quit();
		return;
	}

	::ImGui::ShowDemoWindow();

	Windowing::WindowService& windowing = services->GetService<Windowing::WindowService>();
	Rendering::RenderService& rendering = services->GetService<Rendering::RenderService>();
	
	std::vector<Ref<Windowing::Window>> visibleWindows = windowing.GetVisibleWindows();
	std::array<SceneDataProvider*, 1> dataProviders = { _sceneDataProvider.get()};

	for (const Ref<Windowing::Window>& window : visibleWindows)
	{
		rendering.Render(window->GetPresenter(), *_pipeline, *_renderViewProvider, dataProviders);
	}
}