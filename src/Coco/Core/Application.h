#pragma once

#include "Core.h"
#include "Logging/Logger.h"
#include "Events/Event.h"

namespace Coco
{
	class Engine;

	/// @brief Base class for a client application that the engine runs
	class COCOAPI Application
	{
	public:
		/// @brief The name of the application
		const string Name;

		/// @brief Invoked when the application is trying to quit. Setting the bool value to true means the quit is cancelled
		Event<bool&> OnQuitting;

		/// @brief Invoked when the application has quit
		Event<> OnQuit;

		/// @brief A pointer the engine
		Engine* const Engine;

	protected:
		Managed<Logging::Logger> Logger;

	protected:
		Application(Coco::Engine* engine, const string& name);

	public:
		virtual ~Application();

		Application() = delete;
		Application(const Application&) = delete;
		Application(Application&&) = delete;

		Application& operator=(const Application&) = delete;
		Application& operator=(Application&&) = delete;

		/// @brief Creates an application for a given engine
		/// @param engine The engine
		/// @return The created application
		static Managed<Application> Create(Coco::Engine* engine);

		/// @brief Called right before the main loop begins, but after all other engine initialization has happened
		virtual void Start() = 0;

		/// @brief Requests this application to quit
		/// @return True if the application will quit
		bool Quit() noexcept;
	};
}

/// Macro that adds a factory to create the application instance. Should be added in the cpp file for the client application
#define MainApplication(AppClass) Coco::Managed<Coco::Application> CreateApplication(Coco::Engine* engine) { return Coco::CreateManaged<AppClass>(engine); }

