#pragma once
#include <Coco/Core/Engine.h>
#include "Win32EnginePlatformFactory.h"

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR pCmdLine, _In_ int nCmdShow)
{
	// Uncomment for helping debug heap issues
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);

	using namespace Coco;
	using namespace Coco::Platforms::Win32;

	Win32EnginePlatformFactory factory(hInstance);
	UniqueRef<Engine> engine = CreateUniqueRef<Engine>(factory);

	int result = engine->Run();

	engine.reset();

	return result;
}