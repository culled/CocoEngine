#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/ExitCodes.h>
#include <Coco/Core/Types/DateTime.h>
#include <Coco/Core/Logging/Logger.h>
#include <Coco/Core/Platform/EnginePlatform.h>
#include <Coco/Core/Services/EngineServiceManager.h>
#include <Coco/Core/MainLoop/MainLoop.h>

namespace Coco
{
	class Application;

	/// <summary>
	/// The core engine
	/// </summary>
	class COCOAPI Engine
	{
	private:
		struct EngineDeleter
		{
			void operator()(Engine* engine) { delete engine; }
		};

		static Engine* _instance;

		DateTime _startTime;

		ExitCode _exitCode = ExitCode::Ok;
		Managed<Logging::Logger> _logger;
		Managed<Platform::EnginePlatform> _platform;
		Managed<Application> _application;
		Managed<EngineServiceManager> _serviceManager;
		Managed<MainLoop> _mainLoop;

	public:
		/// <summary>
		/// Called from the main entry to run the engine and client application
		/// </summary>
		/// <param name="platform">The platform for the engine to use</param>
		/// <returns>The exit code from the engine</returns>
		static ExitCode Run(Managed<Platform::EnginePlatform> platform);

		/// <summary>
		/// Gets the engine instance
		/// </summary>
		/// <returns>The engine instace</returns>
		static Engine* Get() { return _instance; }

		/// <summary>
		/// Gets the engine's logger
		/// </summary>
		/// <returns>The engine's logger</returns>
		Logging::Logger* GetLogger() const { return _logger.get(); }

		/// <summary>
		/// Gets the engine's platform
		/// </summary>
		/// <returns>The engine's platform</returns>
		Platform::EnginePlatform* GetPlatform() const { return _platform.get(); }

		/// <summary>
		/// Gets the client application
		/// </summary>
		/// <returns>The client application</returns>
		Application* GetApplication() const { return _application.get(); }

		/// <summary>
		/// Gets the service manager
		/// </summary>
		/// <returns>The service manager</returns>
		EngineServiceManager* GetServiceManager() const { return _serviceManager.get(); }

		/// <summary>
		/// Gets the main loop
		/// </summary>
		/// <returns>The main loop</returns>
		MainLoop* GetMainLoop() const { return _mainLoop.get(); }

		/// <summary>
		/// Sets the exit code for the engine. Will be used unless an error occurs
		/// </summary>
		/// <param name="code">The exit code</param>
		void SetExitCode(ExitCode code) { _exitCode = code; }

		/// <summary>
		/// Gets the current exit code for the engine
		/// </summary>
		/// <returns>The current exit code</returns>
		ExitCode GetExitCode() const { return _exitCode; }

		/// <summary>
		/// Gets the amount of time the engine has been running
		/// </summary>
		/// <returns>The amount of time the engine has been running</returns>
		struct TimeSpan GetRunningTime() const;

	private:
		Engine(Managed<Platform::EnginePlatform> platform);
		~Engine();

		/// <summary>
		/// Runs the client application
		/// </summary>
		/// <returns>The return code from the application</returns>
		ExitCode Run();
	};
}

