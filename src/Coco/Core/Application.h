#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Logging/Logger.h>

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

	protected:
		Engine* Engine;
		Managed<Logging::Logger> Logger;

	protected:
		Application(Coco::Engine* engine, const string& name);

	public:
		/// <summary>
		/// Creates an application for a given engine
		/// </summary>
		/// <param name="engine">The engine</param>
		/// <returns>The created application</returns>
		static Managed<Application> Create(Coco::Engine* engine);

		virtual ~Application();

		/// <summary>
		/// Called right before the main loop begins, but after all other engine initialization has happened
		/// </summary>
		virtual void Start() = 0;

		/// <summary>
		/// Requests this application to quit
		/// </summary>
		/// <returns>True if the application will quit</returns>
		bool Quit();
	};
}

/// Macro that adds a factory to create the application instance. Should be added in the cpp file for the client application
#define MainApplication(AppClass) Coco::Managed<Coco::Application> CreateApplication(Coco::Engine* engine) { return Coco::CreateManaged<AppClass>(engine); }

