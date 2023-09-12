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

		ApplicationCreateParameters(const string& name, const Coco::Version& version);
	};

	/// @brief Base class for an application run by the Engine
	class Application : public Singleton<Application>
	{
	protected:
		ApplicationCreateParameters _createParams;
		UniqueRef<Log> _log;

	protected:
		Application(const ApplicationCreateParameters& createParams);

	public:
		virtual ~Application() = default;

		/// @brief Gets the name of this application
		/// @return The application name
		const string& GetName() const { return _createParams.Name; }

		/// @brief Gets the version of this application
		/// @return The application version
		Version GetVersion() const { return _createParams.Version; }

		Log* GetLog() { return _log.get(); }
		const Log* GetLog() const { return _log.get(); }
	};
}

/// @brief Creates an application instance.
/// NOTE: This should be defined somewhere in the application itself
/// @return The application
Coco::Application* CreateApplication();

/// @brief Basic macro for defining an application class type
#define MainApplication(AppClass) Coco::Application* CreateApplication() { return new AppClass(); }