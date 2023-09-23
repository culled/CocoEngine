#pragma once

#include "Core.h"
#include "Application.h"
#include "Platform/EnginePlatformFactory.h"
#include "MainLoop/MainLoop.h"
#include "Services/ServiceManager.h"
#include "IO/EngineFileSystem.h"
#include "Resources/ResourceLibrary.h"

namespace Coco
{
	/// @brief The main engine class
	class Engine : public Singleton<Engine>
	{
	public:
		static const char* sShowConsoleArgument;
		static const char* sContentPathArgument;
		static const char* sDefaultContentPath;

	private:
		UniqueRef<Log> _log;
		UniqueRef<EnginePlatform> _platform;
		UniqueRef<MainLoop> _mainLoop;
		UniqueRef<ServiceManager> _serviceManager;
		UniqueRef<EngineFileSystem> _fileSystem;
		UniqueRef<ResourceLibrary> _resourceLibrary;
		UniqueRef<Application> _app;
		int _exitCode;

	public:
		Engine(const EnginePlatformFactory& platformFactory);
		~Engine();

		/// @brief Gets the version of this engine
		/// @return The engine version
		Version GetVersion() const { return Version(0, 0, 1); }

		/// @brief Runs the application and returns the exit code
		/// @return The exit code
		int Run();

		/// @brief Sets the exit code when the engine exits
		/// @param code The exit code
		void SetExitCode(int code);

		/// @brief Gets the platform that the engine is running on
		/// @return The platform
		EnginePlatform& GetPlatform() { return *_platform; }

		/// @brief Gets the platform that the engine is running on
		/// @return The platform
		const EnginePlatform& GetPlatform() const { return *_platform; }

		/// @brief Gets the application that the engine is running
		/// @return The application
		Application& GetApp() { return *_app; }

		/// @brief Gets the application that the engine is running
		/// @return The application
		const Application& GetApp() const { return *_app; }

		/// @brief Gets the engine's log
		/// @return The engine's log
		Log& GetLog() { return *_log; }

		/// @brief Gets the engine's log
		/// @return The engine's log
		const Log& GetLog() const { return *_log; }

		/// @brief Gets the engine's main loop
		/// @return The main loop
		MainLoop& GetMainLoop() { return *_mainLoop; }

		/// @brief Gets the engine's main loop
		/// @return The main loop
		const MainLoop& GetMainLoop() const { return *_mainLoop; }

		/// @brief Gets the engine's service manager
		/// @return The service manager
		ServiceManager& GetServiceManager() { return *_serviceManager; }

		/// @brief Gets the engine's service manager
		/// @return The service manager
		const ServiceManager& GetServiceManager() const { return *_serviceManager; }

		/// @brief Gets the engine's file system
		/// @return The file system
		EngineFileSystem& GetFileSystem() { return *_fileSystem; }

		/// @brief Gets the engine's file system
		/// @return The file system
		const EngineFileSystem& GetFileSystem() const { return *_fileSystem; }

		/// @brief Gets the engine's resource library
		/// @return The resource library
		ResourceLibrary& GetResourceLibrary() { return *_resourceLibrary; }

		/// @brief Gets the engine's resource library
		/// @return The resource library
		const ResourceLibrary& GetResourceLibrary() const { return *_resourceLibrary; }

		/// @brief Causes a crash and rethrows the captured exception.
		/// NOTE: only works in the catch clause of a try-catch block
		void CrashWithException();

	private:
		/// @brief Performs setup based on process arguments
		void SetupFromProcessArguments();
	};
}

#ifdef COCO_LOG_TRACE
#define CocoTrace(Message, ...) ::Coco::Engine::Get()->GetLog().FormatWrite(Coco::LogMessageSeverity::Trace, Message, __VA_ARGS__);
#else
#define CocoTrace(Message, ...)
#endif

#ifdef COCO_LOG_INFO
#define CocoInfo(Message, ...) ::Coco::Engine::Get()->GetLog().FormatWrite(Coco::LogMessageSeverity::Info, Message, __VA_ARGS__);
#else
#define CocoInfo(Message, ...)
#endif

#ifdef COCO_LOG_WARNING
#define CocoWarn(Message, ...) ::Coco::Engine::Get()->GetLog().FormatWrite(Coco::LogMessageSeverity::Warning, Message, __VA_ARGS__);
#else
#define CocoWarn(Message, ...)
#endif

#define CocoError(Message, ...) ::Coco::Engine::Get()->GetLog().FormatWrite(Coco::LogMessageSeverity::Error, Message, __VA_ARGS__);
#define CocoCritical(Message, ...) ::Coco::Engine::Get()->GetLog().FormatWrite(Coco::LogMessageSeverity::Critical, Message, __VA_ARGS__);