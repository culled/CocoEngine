#include "Corepch.h"
#include "Engine.h"

#include "Logging/LogSinks/ConsoleLogSink.h"

namespace Coco
{
	const char* Engine::sShowConsoleArgument = "--show-console";

	Engine::Engine(const EnginePlatformFactory& platformFactory)
	{
		_platform = platformFactory.Create();
		_log = CreateUniqueRef<Log>("Coco", LogMessageSeverity::Trace);

		SetupFromProcessArguments();

		_app.reset(CreateApplication());

		LogTrace(_log, "Engine initialized")
	}

	Engine::~Engine()
	{
		_app.reset();

		LogTrace(_log, "Engine shutdown. Bye!")
		_log.reset();

		_platform.reset();
	}

	int Engine::Run()
	{
		// Early exit if there was an error during setup
		if (_exitCode != 0)
			return _exitCode;

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