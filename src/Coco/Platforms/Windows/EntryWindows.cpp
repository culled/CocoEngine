#include <Coco/Core/Core.h>

#include "WindowsIncludes.h"

#include <Coco/Core/Engine.h>
#include "EnginePlatformWindows.h"

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR pCmdLine, _In_ int nCmdShow)
{
	using namespace Coco;
	return static_cast<int>(Engine::Run(CreateManaged<Platform::Windows::EnginePlatformWindows>(hInstance, hPrevInstance, pCmdLine, nCmdShow)));
}