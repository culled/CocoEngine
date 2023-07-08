#include "Application.h"

#include <Coco/Core/Engine.h>
#include <Coco/Core/Logging/Logger.h>
#include <Coco/Core/MainLoop/MainLoop.h>

// The factory for creating our main application class. Should be defined using the CreateApplication macro
extern Coco::ManagedRef<Coco::Application> CreateApplication();

namespace Coco
{
	Application::Application(const string& name) :
		Name(name), _logger(CreateManagedRef<Logging::Logger>(name))
	{}

	Application::~Application()
	{
		_logger.Reset();
	}

	ManagedRef<Application> Application::Create()
	{
		return CreateApplication();
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

		Engine::Get()->GetMainLoop()->Stop();

		return true;
	}
}