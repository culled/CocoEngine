#include "Corepch.h"
#include "Engine.h"

#include "Logging/LogSinks/ConsoleLogSink.h"

namespace Coco
{
	const char* Engine::sShowConsoleArgument = "--show-console";

	Engine::Engine(const EnginePlatformFactory& platformFactory) :
		_exitCode(0)
	{
		_platform = platformFactory.Create();
		_log = CreateUniqueRef<Log>("Coco", LogMessageSeverity::Trace);

		SetupFromProcessArguments();

		_mainLoop = CreateUniqueRef<MainLoop>();
		_serviceManager = CreateUniqueRef<ServiceManager>();

		try
		{
			_app.reset(CreateApplication());
		}
		catch (const std::exception& ex)
		{
			SetExitCode(-1);
			LogCritical(_log, "Error Creating Application", ex.what(), true)
			_platform->ShowMessageBox("Error Creating Application", ex.what(), true);
			DebuggerBreak
		}

		LogTrace(_log, "Engine initialized")
	}

	Engine::~Engine()
	{
		_app.reset();
		_serviceManager.reset();
		_mainLoop.reset();

		LogTrace(_log, "Engine shutdown. Bye!")
		_log.reset();

		_platform.reset();
	}

	int Engine::Run()
	{
		// Early exit if there was an error during setup
		if (_exitCode != 0)
		{
			LogCritical(_log, "There was an error during setup, stopping run...")
			return _exitCode;
		}

		_mainLoop->Run();

		return _exitCode;
	}

	void Engine::SetExitCode(int code)
	{
		_exitCode = code;
	}

	void Engine::SetupFromProcessArguments()
	{
		if (_platform->HasProcessArgument(sShowConsoleArgument))
		{
			_platform->ShowConsoleWindow(true);

			_log->AddSink(CreateSharedRef<ConsoleLogSink>());
		}
	}
}