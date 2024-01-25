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
		Assert(_platform)
		Assert(_log)

		SetupFromProcessArguments();

		_mainLoop = CreateUniqueRef<MainLoop>();
		_resourceLibrary = CreateUniqueRef<ResourceLibrary>();
		_assetManager = CreateUniqueRef<AssetManager>();
		_serviceManager = CreateUniqueRef<ServiceManager>();

		try
		{
			_app.reset(CreateApplication());
		}
		catch (const std::exception& ex)
		{
			Crash(FormatString("Error creating application: {}", ex.what()));
		}
		catch (...)
		{
			Crash("Unknown error creating application");
		}

		if (_exitCode != -1)
		{
			LogTrace(_log, "Engine initialized")
		}
	}

	Engine::~Engine()
	{
		_app.reset();
		_assetManager.reset();
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
		}
		catch (const std::exception& ex)
		{
			Crash(ex);
		}
		catch (...)
		{
			Crash("Unknown runtime error");
		}

		return _exitCode;
	}

	void Engine::SetExitCode(int code)
	{
		_exitCode = code;
	}

	void Engine::Crash(const string& error)
	{
		LogCritical(_log, "Critical Runtime Error: {}", error)
		_platform->ShowMessageBox("Critical Runtime Error", error.c_str(), true);
		DebuggerBreak

		SetExitCode(-1);
		_mainLoop->Stop();

		// *Pulls out the knife*
		abort();
	}

	void Engine::Crash(const std::exception& ex)
	{
		Crash(ex.what());
	}

	void Engine::SetupFromProcessArguments()
	{
		if (_platform->HasProcessArgument(sShowConsoleArgument))
		{
			try
			{
				_platform->ShowConsoleWindow(true);

				_log->AddSink(CreateSharedRef<ConsoleLogSink>());
			}
			catch (const std::exception& ex)
			{
				CocoError("Failed to create console log: {}", ex.what())
			}
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

		// TODO: temporary! We should figure out which file system to use based on the environment we're running in
		_fileSystem = CreateUniqueRef<UnpackedEngineFileSystem>(contentPath);
		CocoAssert(_fileSystem, "File system could not be created")
	}
}