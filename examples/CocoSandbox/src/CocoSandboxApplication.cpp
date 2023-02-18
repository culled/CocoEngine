#include "CocoSandboxApplication.h"

#include <Coco/Core/Engine.h>
#include <Coco/Core/Logging/Logger.h>
#include <Coco/Core/Logging/Sinks/ConsoleLogSink.h>
#include <Coco/Core/Services/EngineServiceManager.h>
#include <Coco/Core/Types/Size.h>
#include <Coco/Windowing/Window.h>
#include <Coco/Windowing/WindowingService.h>
#include <Coco/Core/Input/InputService.h>
#include <Coco/Core/Input/Keyboard.h>
#include <Coco/Core/Input/Mouse.h>
#include <Coco/Core/MainLoop/MainLoopTickListener.h>
#include <Coco/Core/MainLoop/MainLoop.h>

MainApplication(CocoSandboxApplication)

using namespace Coco;

CocoSandboxApplication::CocoSandboxApplication(Coco::Engine* engine) : 
	Coco::Application(engine, "Coco Sandbox"),
	_window(nullptr),
	_tickListener(new Coco::MainLoopTickListener(this, &CocoSandboxApplication::Tick, 0))
{
	Ref<Logging::ConsoleLogSink> consoleSink = CreateRef<Logging::ConsoleLogSink>(Logging::LogLevel::Trace);
	Logger->AddSink(consoleSink);
	engine->GetLogger()->AddSink(consoleSink);

	_windowService = engine->GetServiceManager()->CreateService<Windowing::WindowingService>();
	_inputService = engine->GetServiceManager()->CreateService<Input::InputService>();

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

	_inputService->GetKeyboard()->OnKeyPressedEvent += [&](Input::Keyboard::Key key) {
		if (key == Input::Keyboard::Key::Escape)
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
	if (_inputService->GetKeyboard()->WasKeyJustPressed(Input::Keyboard::Key::Q))
	{
		LogInfo(Logger, "Q pressed");
	}

	if (_inputService->GetKeyboard()->WasKeyJustReleased(Input::Keyboard::Key::E))
	{
		LogInfo(Logger, "E released");
	}

	if (_inputService->GetMouse()->WasButtonJustPressed(Input::Mouse::Button::Left))
	{
		LogInfo(Logger, "Left mouse button pressed");
	}

	if (_inputService->GetMouse()->WasButtonJustReleased(Input::Mouse::Button::Right))
	{
		LogInfo(Logger, "Right mouse button released");
	}
}
