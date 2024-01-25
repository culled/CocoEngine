#include "Corepch.h"
#include "Application.h"

#include "Engine.h"
namespace Coco
{
	ApplicationCreateParameters::ApplicationCreateParameters(
		const string& name,
		const Coco::Version& version,
		LogMessageSeverity logSeverity,
		bool copyEngineLogSinks):
		Name(name), 
		Version(version),
		LogSeverity(logSeverity),
		CopyEngineLogSinks(copyEngineLogSinks)
	{}

	Application::Application(const ApplicationCreateParameters& createParams) :
		_createParams(createParams),
		_log(CreateUniqueRef<Log>(createParams.Name, createParams.LogSeverity))
	{
		CocoAssert(_log, "Application log could not be created")

		if(createParams.CopyEngineLogSinks)
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