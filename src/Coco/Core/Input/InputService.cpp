#include "InputService.h"

#include <Coco/Core/Engine.h>
#include <Coco/Core/MainLoop/MainLoop.h>
#include "Keyboard.h"
#include "Mouse.h"

namespace Coco::Input
{
	InputService::InputService() :
		_keyboard(new Keyboard()),
		_mouse(new Mouse()),
		_tickListener(new MainLoopTickListener(this, &InputService::Tick, InputTickPriority))
	{}

	InputService::~InputService()
	{
		Engine::Get()->GetMainLoop()->RemoveTickListener(_tickListener);
		_tickListener.reset();

		_keyboard.reset();
		_mouse.reset();
	}

	Logging::Logger* InputService::GetLogger() const
	{
		return Engine::Get()->GetLogger();
	}

	void InputService::Start()
	{
		Engine::Get()->GetMainLoop()->AddTickListener(_tickListener);
	}

	void InputService::Tick(double deltaTime)
	{
		_keyboard->SavePreviousState();
		_mouse->SavePreviousState();
	}
}