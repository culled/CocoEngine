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

	Managed<Application> Application::Create(Coco::Engine* engine)
	{
		return CreateApplication(engine);
	}

	Application::~Application()
	{
		Logger.reset();
	}

	bool Application::Quit()
	{
		bool cancelQuit = false;
		OnQuitting.Invoke(&cancelQuit);

		if (cancelQuit)
			return false;

		OnQuit.InvokeEvent();
		Engine->GetMainLoop()->Stop();

		return true;
	}
}