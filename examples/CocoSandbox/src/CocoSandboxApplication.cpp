#include "CocoSandboxApplication.h"

#include <Coco/Core/Types/Size.h>
#include <Coco/Core/Engine.h>
#include <Coco/Core/Logging/Sinks/ConsoleLogSink.h>
#include <Coco/Core/Input/InputService.h>
#include <Coco/Windowing/WindowingService.h>
#include <Coco/Rendering/RenderingService.h>

MainApplication(CocoSandboxApplication)

using namespace Coco;

CocoSandboxApplication::CocoSandboxApplication(Coco::Engine* engine) : 
	Coco::Application(engine, "Coco Sandbox"),
	_tickListener(new Coco::MainLoopTickListener(this, &CocoSandboxApplication::Tick, 0))
{
	Ref<Logging::ConsoleLogSink> consoleSink = CreateRef<Logging::ConsoleLogSink>(Logging::LogLevel::Trace);
	Logger->AddSink(consoleSink);
	engine->GetLogger()->AddSink(consoleSink);

	_inputService = engine->GetServiceManager()->CreateService<Input::InputService>();

	Rendering::GraphicsBackendCreationParameters createParams(Name, Rendering::RenderingRHI::Vulkan);
	_renderService = engine->GetServiceManager()->CreateService<Rendering::RenderingService>(createParams);

	_windowService = engine->GetServiceManager()->CreateService<Windowing::WindowingService>();

	//engine->GetMainLoop()->SetTargetTickRate(60);

	LogInfo(Logger, "Sandbox application created");
}

CocoSandboxApplication::~CocoSandboxApplication()
{
	this->Engine->GetMainLoop()->RemoveTickListener(_tickListener);
	_tickListener.reset();

	LogInfo(Logger, "Sandbox application destroyed");
}

void CocoSandboxApplication::Start()
{
	Windowing::WindowCreateParameters windowCreateParams("Coco Sandbox", SizeInt(1280, 720));
	_window = _windowService->CreateNewWindow(windowCreateParams);
	_window->Show();

	_inputService->GetKeyboard()->OnKeyPressedEvent += [&](Input::KeyboardKey key) {
		//LogInfo(Logger, FormattedString("Pressed key {}", static_cast<int>(key)));
		if (key == Input::KeyboardKey::Escape)
		{
			Quit();
			return true;
		}

		return false;
		};

	this->Engine->GetMainLoop()->AddTickListener(_tickListener);

	LogInfo(Logger, "Sandbox application started");
}

void CocoSandboxApplication::Tick(double deltaTime)
{
	//LogInfo(Logger, FormattedString("Tick! Time = {} (Dt = {})", Engine->GetMainLoop()->GetRunningTime(), deltaTime));
}
