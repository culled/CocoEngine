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
	/// @brief The core engine
	class COCOAPI Engine
	{
	private:
		static Engine* _instance;

		DateTime _startTime;

		ExitCode _exitCode = ExitCode::Ok;
		ManagedRef<Logging::Logger> _logger;
		Platform::IEnginePlatform* _platform;
		ManagedRef<Application> _application;
		ManagedRef<EngineServiceManager> _serviceManager;
		ManagedRef<MainLoop> _mainLoop;
		ManagedRef<ResourceLibrary> _resourceLibrary;

	public:
		Engine(Platform::IEnginePlatform* platform);
		virtual ~Engine();

		Engine() = delete;
		Engine(const Engine&) = delete;
		Engine(Engine&&) = delete;

		Engine& operator=(const Engine&) = delete;
		Engine& operator=(Engine&&) = delete;

		/// @brief Called from the main entry to run the engine and client application
		/// @param platform The platform for the engine to use
		/// @return The exit code from the engine
		static ExitCode Run(ManagedRef<Platform::IEnginePlatform> platform);

		/// @brief Gets the engine instance
		/// @return The engine instace
		static Engine* Get() noexcept { return _instance; }

		/// @brief Gets the engine's logger
		/// @return The engine's logger
		Logging::Logger* GetLogger() noexcept { return _logger.Get(); }

		/// @brief Gets the engine's platform
		/// @return The engine's platform
		Platform::IEnginePlatform* GetPlatform() noexcept { return _platform; }

		/// @brief Gets the client application
		/// @return The client application
		Application* GetApplication() noexcept { return _application.Get(); }

		/// @brief Gets the service manager
		/// @return The service manager
		EngineServiceManager* GetServiceManager() noexcept { return _serviceManager.Get(); }

		/// @brief Gets the main loop
		/// @return The main loop
		MainLoop* GetMainLoop() noexcept { return _mainLoop.Get(); }

		/// @brief Gets the resource library
		/// @return The resource library
		ResourceLibrary* GetResourceLibrary() noexcept { return _resourceLibrary.Get(); }

		/// @brief Sets the exit code for the engine. Will be used unless an error occurs
		/// @param code The exit code
		void SetExitCode(ExitCode code) noexcept { _exitCode = code; }

		/// @brief Gets the current exit code for the engine
		/// @return The current exit code
		ExitCode GetExitCode() const noexcept { return _exitCode; }

		/// @brief Gets the amount of time the engine has been running
		/// @return The amount of time the engine has been running
		TimeSpan GetRunningTime() const noexcept;

	private:
		/// @brief Runs the client application
		/// @return The return code from the application
		ExitCode Run();
	};
}

