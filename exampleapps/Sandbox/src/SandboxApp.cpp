#include "SandboxApp.h"
#include <Coco/Platforms/Win32/EntryPoint.h>

using namespace Coco;

MainApplication(SandboxApp)

SandboxApp::SandboxApp() : 
	Application(ApplicationCreateParameters("Sandbox", Version(0, 0, 1))),
	_tickListener(this, &SandboxApp::Tick, 0)
{

	Engine::Get()->GetMainLoop()->AddListener(_tickListener);
	Engine::Get()->GetMainLoop()->SetTargetTicksPerSecond(2);

	LogTrace(_log, "Sandbox app initialized")
}

SandboxApp::~SandboxApp()
{
	LogTrace(_log, "Sandbox app shutdown")
}

void SandboxApp::Tick(const TickInfo& tickInfo)
{
	LogInfo(_log, "Tick {} ({})", tickInfo.TickNumber, tickInfo.DeltaTime)

	if (tickInfo.TickNumber > 9)
	{
		Engine::Get()->GetMainLoop()->Stop();
	}
}
