#pragma once

#include "Core.h"
#include "Application.h"
#include "Platform/EnginePlatformFactory.h"

namespace Coco
{
	/// @brief The main engine class
	class Engine : public Singleton<Engine>
	{
	public:
		static const char* sShowConsoleArgument;

	private:
		UniqueRef<Log> _log;
		UniqueRef<EnginePlatform> _platform;
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

		EnginePlatform* GetPlatform() { return _platform.get(); }
		const EnginePlatform* GetPlatform() const { return _platform.get(); }

		Application* GetApp() { return _app.get(); }
		const Application* GetApp() const { return _app.get(); }

		Log* GetLog() { return _log.get(); }
		const Log* GetLog() const { return _log.get(); }

	private:
		void SetupFromProcessArguments();
	};
}

#ifdef COCO_LOG_TRACE
#define CocoTrace(Message, ...) ::Coco::Engine::Get()->GetLog()->FormatWrite(Coco::LogMessageSeverity::Trace, Message, __VA_ARGS__);
#else
#define CocoTrace(Message, ...)
#endif

#ifdef COCO_LOG_INFO
#define CocoInfo(Message, ...) ::Coco::Engine::Get()->GetLog()->FormatWrite(Coco::LogMessageSeverity::Info, Message, __VA_ARGS__);
#else
#define CocoInfo(Message, ...)
#endif

#ifdef COCO_LOG_WARNING
#define CocoWarn(Message, ...) ::Coco::Engine::Get()->GetLog()->FormatWrite(Coco::LogMessageSeverity::Warning, Message, __VA_ARGS__);
#else
#define CocoWarn(Message, ...)
#endif

#define CocoError(Message, ...) ::Coco::Engine::Get()->GetLog()->FormatWrite(Coco::LogMessageSeverity::Error, Message, __VA_ARGS__);
#define CocoCritical(Message, ...) ::Coco::Engine::Get()->GetLog()->FormatWrite(Coco::LogMessageSeverity::Critical, Message, __VA_ARGS__);