#include "Corepch.h"
#include "Engine.h"

#include "Logging/LogSinks/ConsoleLogSink.h"
#include "IO/FileSystems/UnpackedEngineFileSystem.h"

namespace Coco
{
	const char* Engine::sShowConsoleArgument = "--show-console";
	const char* Engine::sContentPathArgument = "--content-path";
	const char* Engine::sDefaultContentPath = "assets";

	Engine::Engine(const EnginePlatformFactory& platformFactory) :
		_exitCode(0),
		_platform(platformFactory.Create()),
		_log(CreateUniqueRef<Log>("Coco", LogMessageSeverity::Info))
	{
		SetupFromProcessArguments();

		_mainLoop = CreateUniqueRef<MainLoop>();
		_resourceLibrary = CreateUniqueRef<ResourceLibrary>();
		_serviceManager = CreateUniqueRef<ServiceManager>();

		try
		{
			_app.reset(CreateApplication());
		}
		catch (const std::exception& ex)
		{
			SetExitCode(-1);
			LogCritical(_log, "Error creating application: {}", ex.what())
			_platform->ShowMessageBox("Error Creating Application", ex.what(), true);
			DebuggerBreak
		}

		LogTrace(_log, "Engine initialized")
	}

	Engine::~Engine()
	{
		_app.reset();
		_serviceManager.reset();
		_resourceLibrary.reset();
		_mainLoop.reset();
		_fileSystem.reset();

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

		try
		{
			_app->Start();
			_mainLoop->Run();
			return _exitCode;
		}
		catch (...)
		{
			CrashWithException();
			return -1;
		}
	}

	void Engine::SetExitCode(int code)
	{
		_exitCode = code;
	}

	void Engine::CrashWithException()
	{
		try
		{
			throw;
		}
		catch (const std::exception& ex)
		{
			LogCritical(_log, "Uncaught runtime exception: {}", ex.what())
			_platform->ShowMessageBox("Uncaught Runtime Exception", ex.what(), true);
			DebuggerBreak
		}
	}

	void Engine::SetupFromProcessArguments()
	{
		if (_platform->HasProcessArgument(sShowConsoleArgument))
		{
			_platform->ShowConsoleWindow(true);

			_log->AddSink(CreateSharedRef<ConsoleLogSink>());
		}

		FilePath contentPath;

		if (_platform->HasProcessArgument(sContentPathArgument))
		{
			contentPath = FilePath(_platform->GetProcessArgument(sContentPathArgument));
		}
		else
		{
			contentPath = FilePath::CombineToPath(FilePath::GetCurrentWorkingDirectoryPath(), sDefaultContentPath);
		}

		_fileSystem = CreateUniqueRef<UnpackedEngineFileSystem>(contentPath);
	}
}