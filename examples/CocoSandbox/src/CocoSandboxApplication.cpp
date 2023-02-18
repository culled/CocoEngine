#include "CocoSandboxApplication.h"

#include <Coco/Core/Engine.h>
#include <Coco/Core/Logging/Logger.h>
#include <Coco/Core/Logging/Sinks/ConsoleLogSink.h>
#include <Coco/Core/Services/EngineServiceManager.h>
#include <Coco/Core/Types/Size.h>
#include <Coco/Windowing/Window.h>
#include <Coco/Windowing/WindowingService.h>

MainApplication(CocoSandboxApplication)

using namespace Coco;

CocoSandboxApplication::CocoSandboxApplication(Coco::Engine* engine) : 
	Coco::Application(engine, "Coco Sandbox"), _window(nullptr)
{
	Ref<Logging::ConsoleLogSink> consoleSink = CreateRef<Logging::ConsoleLogSink>(Logging::LogLevel::Trace);
	Logger->AddSink(consoleSink);
	engine->GetLogger()->AddSink(consoleSink);

	_windowService = engine->GetServiceManager()->CreateService<Windowing::WindowingService>();

	_query.AddHandler(this, &CocoSandboxApplication::QueryHandler);
	_event.AddHandler(this, &CocoSandboxApplication::EventHandler);

	int i = 0;
	_query.Invoke(&i, 9);
	_event.InvokeEvent(i);

	LogInfo(Logger, FormattedString("Query value is {}", i));

	LogInfo(Logger, "Sandbox application created");
}

CocoSandboxApplication::~CocoSandboxApplication()
{
	LogInfo(Logger, "Sandbox application destroyed");
}

void CocoSandboxApplication::Start()
{
	Windowing::WindowCreateParameters windowCreateParams("Coco Sandbox", SizeInt(1280, 720));
	_window = _windowService->CreateNewWindow(windowCreateParams);
	_window->Show();

	LogInfo(Logger, "Sandbox application started");
}

int CocoSandboxApplication::QueryHandler(int param)
{
	LogInfo(Logger, FormattedString("Query 1 with param {}!", param));
	return param + 1;
}

bool CocoSandboxApplication::EventHandler(int& param)
{
	LogInfo(Logger, FormattedString("Event 1 with param {}!", param));
	param = 1000;
	return true;
}
