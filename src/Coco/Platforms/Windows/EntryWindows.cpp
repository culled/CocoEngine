#include <Coco/Core/Core.h>

#include "WindowsIncludes.h"

#include <Coco/Core/Engine.h>
#include "EnginePlatformWindows.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
	using namespace Coco;
	Managed<Platform::Windows::EnginePlatformWindows> platform = CreateManaged<Platform::Windows::EnginePlatformWindows>(hInstance, hPrevInstance, pCmdLine, nCmdShow);
	return static_cast<int>(Engine::Run(std::move(platform)));
}