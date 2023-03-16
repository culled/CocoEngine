#include "Application.h"

#include <Coco/Core/Engine.h>
#include <Coco/Core/Logging/Logger.h>
#include <Coco/Core/MainLoop/MainLoop.h>

// The factory for creating our main application class. Should be defined using the CreateApplication macro
extern Coco::Managed<Coco::Application> CreateApplication(Coco::Engine* engine);

namespace Coco
{
	Application::Application(Coco::Engine* engine, const string& name) :
		Engine(engine), Name(name)
	{
		Logger = CreateManaged<Logging::Logger>(name);
	}

	Application::~Application()
	{
		Logger.reset();
	}


	/// <summary>
	/// Creates an application for a given engine
	/// </summary>
	/// <param name="engine">The engine</param>
	/// <returns>The created application</returns>

	Managed<Application> Application::Create(Coco::Engine* engine)
	{
		return CreateApplication(engine);
	}

	bool Application::Quit() noexcept
	{
		bool cancelQuit = false;

		try
		{
			OnQuitting.Invoke(cancelQuit);
		}
		catch(...)
		{ }

		if (cancelQuit)
			return false;

		try
		{
			OnQuit.Invoke();
		}
		catch(...)
		{ }

		Engine->GetMainLoop()->Stop();

		return true;
	}
}