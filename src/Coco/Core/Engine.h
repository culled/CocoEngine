#pragma once

#include "Core.h"
#include "ExitCodes.h"
#include "Types/DateTime.h"
#include "Types/TimeSpan.h"
#include "Logging/Logger.h"
#include "Platform/IEnginePlatform.h"
#include "Services/EngineServiceManager.h"
#include "MainLoop/MainLoop.h"
#include "Resources/ResourceLibrary.h"
#include "Application.h"

namespace Coco
{
	/// <summary>
	/// The core engine
	/// </summary>
	class COCOAPI Engine
	{
	private:
		static Engine* _instance;

		DateTime _startTime;

		ExitCode _exitCode = ExitCode::Ok;
		Managed<Logging::Logger> _logger;
		Platform::IEnginePlatform* _platform;
		Managed<Application> _application;
		Managed<EngineServiceManager> _serviceManager;
		Managed<MainLoop> _mainLoop;
		Managed<ResourceLibrary> _resourceLibrary;

	public:
		Engine(Platform::IEnginePlatform* platform);
		virtual ~Engine();

		Engine() = delete;
		Engine(const Engine&) = delete;
		Engine(Engine&&) = delete;

		Engine& operator=(const Engine&) = delete;
		Engine& operator=(Engine&&) = delete;

		/// <summary>
		/// Called from the main entry to run the engine and client application
		/// </summary>
		/// <param name="platform">The platform for the engine to use</param>
		/// <returns>The exit code from the engine</returns>
		static ExitCode Run(Managed<Platform::IEnginePlatform> platform);

		/// <summary>
		/// Gets the engine instance
		/// </summary>
		/// <returns>The engine instace</returns>
		static Engine* Get() noexcept { return _instance; }

		/// <summary>
		/// Gets the engine's logger
		/// </summary>
		/// <returns>The engine's logger</returns>
		Logging::Logger* GetLogger() const noexcept { return _logger.get(); }

		/// <summary>
		/// Gets the engine's platform
		/// </summary>
		/// <returns>The engine's platform</returns>
		Platform::IEnginePlatform* GetPlatform() const noexcept { return _platform; }

		/// <summary>
		/// Gets the client application
		/// </summary>
		/// <returns>The client application</returns>
		Application* GetApplication() const noexcept { return _application.get(); }

		/// <summary>
		/// Gets the service manager
		/// </summary>
		/// <returns>The service manager</returns>
		EngineServiceManager* GetServiceManager() const noexcept { return _serviceManager.get(); }

		/// <summary>
		/// Gets the main loop
		/// </summary>
		/// <returns>The main loop</returns>
		MainLoop* GetMainLoop() const noexcept { return _mainLoop.get(); }

		/// <summary>
		/// Gets the resource library
		/// </summary>
		/// <returns>The resource library</returns>
		ResourceLibrary* GetResourceLibrary() const noexcept { return _resourceLibrary.get(); }

		/// <summary>
		/// Sets the exit code for the engine. Will be used unless an error occurs
		/// </summary>
		/// <param name="code">The exit code</param>
		void SetExitCode(ExitCode code) noexcept { _exitCode = code; }

		/// <summary>
		/// Gets the current exit code for the engine
		/// </summary>
		/// <returns>The current exit code</returns>
		ExitCode GetExitCode() const noexcept { return _exitCode; }

		/// <summary>
		/// Gets the amount of time the engine has been running
		/// </summary>
		/// <returns>The amount of time the engine has been running</returns>
		TimeSpan GetRunningTime() const noexcept;

	private:
		/// <summary>
		/// Runs the client application
		/// </summary>
		/// <returns>The return code from the application</returns>
		ExitCode Run();
	};
}

