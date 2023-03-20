#include "InputService.h"

#include <Coco/Core/Engine.h>

namespace Coco::Input
{
	InputService::InputService(Coco::Engine* engine) : EngineService(engine),
		_keyboard(CreateManaged<Keyboard>()),
		_mouse(CreateManaged<Mouse>())
	{
		RegisterTickListener(this, &InputService::Process, ProcessTickPriority);
		RegisterTickListener(this, &InputService::LateProcess, LateProcessTickPriority);
	}

	InputService::~InputService()
	{
		_keyboard.reset();
		_mouse.reset();
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