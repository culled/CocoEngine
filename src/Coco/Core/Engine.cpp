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

	ExitCode Engine::Run(ManagedRef<Platform::IEnginePlatform> platform)
	{
		if(platform.Get() == nullptr)
			return ExitCode::FatalError;

		try
		{
			ManagedRef<Engine> engine = CreateManagedRef<Engine>(platform.Get());
			return engine->Run();
		}
		catch (const Exception& ex)
		{
			platform->ShowMessageBox("Fatal error", string(ex.what()), true);
			DebuggerBreak();
			return ExitCode::FatalError;
		}
	}

	TimeSpan Engine::GetRunningTime() const noexcept
	{
		return _platform->GetLocalTime() - _startTime;
	}

	Engine::Engine(Platform::IEnginePlatform* platform) :
		_platform(platform)
	{
		_instance = this;

		_startTime = _platform->GetLocalTime();
		_logger = CreateManagedRef<Logging::Logger>("Coco");
		_serviceManager = CreateManagedRef<EngineServiceManager>(this);
		_mainLoop = CreateManagedRef<MainLoop>(_platform);
		_resourceLibrary = CreateManagedRef<ResourceLibrary>("assets/");

		_application = Application::Create(this);

		LogTrace(_logger, "Engine created");
	}

	Engine::~Engine()
	{
		LogTrace(_logger, "Shutting down engine...");

		_application.Reset();
		_resourceLibrary.Reset();
		_serviceManager.Reset();
		_mainLoop.Reset();
		_logger.Reset();
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
			_platform->ShowMessageBox("Fatal error", string(ex.what()), true);

			DebuggerBreak();
			return ExitCode::FatalError;
		}
	}
}
