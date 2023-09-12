#include "SandboxApp.h"
#include <Coco/Platforms/Win32/EntryPoint.h>

using namespace Coco;

MainApplication(SandboxApp)

SandboxApp::SandboxApp() : 
	Application(ApplicationCreateParameters("Sandbox", Version(0, 0, 1)))
{
	LogTrace(_log, "Sandbox app initialized")
}

SandboxApp::~SandboxApp()
{
	LogTrace(_log, "Sandbox app shutdown")
}