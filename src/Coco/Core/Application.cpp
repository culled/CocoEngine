#include "Corepch.h"
#include "Application.h"

#include "Engine.h"
namespace Coco
{
	ApplicationCreateParameters::ApplicationCreateParameters(const string& name, const Coco::Version& version):
		Name(name), 
		Version(version)
	{}

	Application::Application(const ApplicationCreateParameters& createParams) :
		_createParams(createParams),
		_log(CreateUniqueRef<Log>(createParams.Name, LogMessageSeverity::Trace))
	{
		Engine::cGet()->GetLog()->CopySinksTo(*_log);
	}

	Application::~Application()
	{
		_log.reset();
	}
}