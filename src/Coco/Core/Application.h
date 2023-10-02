#pragma once

#include "Core.h"

namespace Coco
{
	/// @brief Parameters for creating an application
	struct ApplicationCreateParameters
	{
		/// @brief The name of the application
		string Name;

		/// @brief The version of the application
		Version Version;

		/// @brief The minimum message severity for a log message to be logged
		LogMessageSeverity LogSeverity;

		ApplicationCreateParameters(const string& name, const Coco::Version& version);
	};

	/// @brief Base class for an application run by the Engine
	class Application : public Singleton<Application>
	{
		friend class Engine;

	protected:
		ApplicationCreateParameters _createParams;
		UniqueRef<Log> _log;

	protected:
		Application(const ApplicationCreateParameters& createParams);

	public:
		virtual ~Application();

		/// @brief Gets the name of this application
		/// @return The application name
		const char* GetName() const { return _createParams.Name.c_str(); }

		/// @brief Gets the version of this application
		/// @return The application version
		Version GetVersion() const { return _createParams.Version; }

		/// @brief Gets the application's log
		/// @return The application's log
		Log& GetLog() { return *_log; }

		/// @brief Gets the application's log
		/// @return The application's log
		const Log& GetLog() const { return *_log; }

		/// @brief Calls to quit the application
		/// @param exitCode The code to quit with
		void Quit(int exitCode = 0);

	protected:
		/// @brief Called after the engine has completed initialization and right before the main loop starts
		virtual void Start() {}
	};

	/// @brief Creates an application instance.
	/// NOTE: This should be defined somewhere in the application itself
	/// @return The application
	Application* CreateApplication();
}

/// @brief Basic macro for defining an application class type
#define MainApplication(AppClass) Coco::Application* Coco::CreateApplication() { return new AppClass(); }