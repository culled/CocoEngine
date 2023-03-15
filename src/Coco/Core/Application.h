#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Logging/Logger.h>
#include <Coco/Core/Events/Event.h>

// TEMPORARY -----------------------------------------------
#include <Coco/Core/Scene/Components/CameraComponent.h>
// TEMPORARY -----------------------------------------------

namespace Coco
{
	class Engine;

	/// <summary>
	/// Base class for a client application that the engine runs
	/// </summary>
	class COCOAPI Application
	{
	public:
		/// <summary>
		/// The name of the application
		/// </summary>
		const string Name;

		/// <summary>
		/// Invoked when the application is trying to quit.
		/// Setting the bool value to true means the quit is cancelled
		/// </summary>
		Event<bool&> OnQuitting;

		/// <summary>
		/// Invoked when the application has quit
		/// </summary>
		Event<> OnQuit;

	protected:
		Engine* Engine;
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

		/// <summary>
		/// Creates an application for a given engine
		/// </summary>
		/// <param name="engine">The engine</param>
		/// <returns>The created application</returns>
		static Managed<Application> Create(Coco::Engine* engine);

		/// <summary>
		/// Called right before the main loop begins, but after all other engine initialization has happened
		/// </summary>
		virtual void Start() = 0;

		/// <summary>
		/// Requests this application to quit
		/// </summary>
		/// <returns>True if the application will quit</returns>
		bool Quit() noexcept;

		// TEMPORARY -------------------------------------
		virtual Ref<CameraComponent> GetCamera() const noexcept = 0;
		// TEMPORARY -------------------------------------
	};
}

/// Macro that adds a factory to create the application instance. Should be added in the cpp file for the client application
#define MainApplication(AppClass) Coco::Managed<Coco::Application> CreateApplication(Coco::Engine* engine) { return Coco::CreateManaged<AppClass>(engine); }

