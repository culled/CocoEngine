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
	createParams.SupportsPresentation = true;
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

	_inputService->GetKeyboard()->OnKeyPressedEvent += [&](Input::Keyboard::Key key) {
		//LogInfo(Logger, FormattedString("Pressed key {}", static_cast<int>(key)));
		if (key == Input::Keyboard::Key::Escape)
		{
			Quit();
			return true;
		}

		return false;
		};

	//_inputService->GetKeyboard()->OnKeyReleasedEvent += [&](Input::Keyboard::Key key) {
	//	LogInfo(Logger, FormattedString("Released key {}", static_cast<int>(key)));
	//	return false;
	//};

	//_inputService->GetMouse()->OnButtonPressed += [&](Input::Mouse::Button button) {
	//	LogInfo(Logger, FormattedString("Pressed mouse button {}", static_cast<int>(button)));
	//	return false;
	//};

	//_inputService->GetMouse()->OnButtonReleased += [&](Input::Mouse::Button button) {
	//	LogInfo(Logger, FormattedString("Released mouse button {}", static_cast<int>(button)));
	//	return false;
	//};

	//_inputService->GetMouse()->OnMoved += [&](const Vector2Int& newPosition, const Vector2Int& delta) {
	//	LogInfo(Logger, FormattedString("Moved mouse - Position ({}, {}), Delta ({}, {})", newPosition.X, newPosition.Y, delta.X, delta.Y));
	//	return false;
	//};

	//_inputService->GetMouse()->OnScrolled += [&](const Vector2Int& delta) {
	//	LogInfo(Logger, FormattedString("Scrolled mouse - Delta ({}, {})", delta.X, delta.Y));
	//	return false;
	//};

	this->Engine->GetMainLoop()->AddTickListener(_tickListener);

	LogInfo(Logger, "Sandbox application started");
}

void CocoSandboxApplication::Tick(double deltaTime)
{
	//if (_inputService->GetKeyboard()->WasKeyJustPressed(Input::Keyboard::Key::Q))
	//{
	//	LogInfo(Logger, "Q pressed");
	//}
	//
	//if (_inputService->GetKeyboard()->WasKeyJustReleased(Input::Keyboard::Key::E))
	//{
	//	LogInfo(Logger, "E released");
	//}
	//
	//if (_inputService->GetMouse()->WasButtonJustPressed(Input::Mouse::Button::Left))
	//{
	//	LogInfo(Logger, "Left mouse button pressed");
	//}
	//
	//if (_inputService->GetMouse()->WasButtonJustReleased(Input::Mouse::Button::Right))
	//{
	//	LogInfo(Logger, "Right mouse button released");
	//}

	//LogInfo(Logger, FormattedString("Tick! Time = {} (Dt = {})", Engine->GetMainLoop()->GetRunningTime(), deltaTime));
}
