#include "Corepch.h"
#include "Application.h"

#include "Engine.h"
namespace Coco
{
	ApplicationCreateParameters::ApplicationCreateParameters(const string& name, const Coco::Version& version):
		Name(name), 
		Version(version),
		LogSeverity(LogMessageSeverity::Info)
	{}

	Application::Application(const ApplicationCreateParameters& createParams) :
		_createParams(createParams),
		_log(CreateUniqueRef<Log>(createParams.Name, createParams.LogSeverity))
	{
		Engine::cGet()->GetLog().CopySinksTo(*_log);
	}

	Application::~Application()
	{
		_log.reset();
	}

	void Application::Quit(int exitCode)
	{
		MainLoop::Get()->Stop();
	}
}