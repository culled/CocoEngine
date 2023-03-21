#include "Engine.h"

#include <Coco/Core/Application.h>
#include <Coco/Core/Logging/Logger.h>
#include <Coco/Core/Services/EngineServiceManager.h>
#include <Coco/Core/Types/TimeSpan.h>
#include <Coco/Core/MainLoop/MainLoop.h>
#include <Coco/Core/Platform/IEnginePlatform.h>

namespace Coco
{
	Engine* Engine::_instance = nullptr;

	ExitCode Engine::Run(Managed<Platform::IEnginePlatform> platform)
	{
		if(platform.get() == nullptr)
			return ExitCode::FatalError;

		try
		{
			Managed<Engine> engine = CreateManaged<Engine>(platform.get());
			return engine->Run();
		}
		catch (const Exception& ex)
		{
			platform->ShowPlatformMessageBox("Fatal error", string(ex.what()), true);
			DebuggerBreak();
			return ExitCode::FatalError;
		}
	}

	TimeSpan Engine::GetRunningTime() const noexcept
	{
		return _platform->GetPlatformLocalTime() - _startTime;
	}

	Engine::Engine(Platform::IEnginePlatform* platform) :
		_platform(platform)
	{
		_instance = this;
		_startTime = _platform->GetPlatformLocalTime();
		_logger = CreateManaged<Logging::Logger>("Coco");
		_serviceManager = CreateManaged<EngineServiceManager>(this);
		_mainLoop = CreateManaged<MainLoop>(_platform);
		_resourceLibrary = CreateManaged<ResourceLibrary>("assets/");

		_application = Application::Create(this);

		LogTrace(_logger, "Engine created");
	}

	Engine::~Engine()
	{
		LogTrace(_logger, "Shutting down engine...");

		_application.reset();
		_resourceLibrary.reset();
		_serviceManager.reset();
		_mainLoop.reset();
		_logger.reset();
	}

	ExitCode Engine::Run()
	{
		LogTrace(_logger, "Engine starting...");

		try
		{
			_serviceManager->Start();
			_platform->Start();
			_application->Start();

			LogTrace(_logger, "Main loop starting...");

			_mainLoop->Run();

			return _exitCode;
		}
		catch (Exception ex)
		{
			LogFatal(_logger, FormattedString("Fatal error: {0}", ex.what()));
			_platform->ShowPlatformMessageBox("Fatal error", string(ex.what()), true);

			DebuggerBreak();
			return ExitCode::FatalError;
		}
	}
}
