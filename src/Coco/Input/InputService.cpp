#include "InputService.h"

#include <Coco/Core/Engine.h>

namespace Coco::Input
{
	InputService::InputService() : EngineService(),
		_keyboard(CreateManagedRef<Keyboard>()),
		_mouse(CreateManagedRef<Mouse>())
	{
		this->SetSingleton(this);

		MainLoop* mainLoop = Engine::Get()->GetMainLoop();
		mainLoop->CreateTickListener(this, &InputService::Process, ProcessTickPriority);
		mainLoop->CreateTickListener(this, &InputService::LateProcess, LateProcessTickPriority);
	}

	InputService::~InputService()
	{
		_keyboard.Reset();
		_mouse.Reset();
	}

	void InputService::Process(double deltaTime)
	{
		_keyboard->ProcessCurrentState();
		_mouse->ProcessCurrentState();
	}

	void InputService::LateProcess(double deltaTime) noexcept
	{
		_keyboard->SavePreviousState();
		_mouse->SavePreviousState();
	}
}