#include "InputService.h"

#include <Coco/Core/Engine.h>
#include <Coco/Core/MainLoop/MainLoop.h>

namespace Coco::Input
{
	InputService::InputService() :
		_keyboard(new Keyboard()),
		_mouse(new Mouse()),
		_processListener(new MainLoopTickListener(this, &InputService::Process, ProcessTickPriority)),
		_lateProcessListener(new MainLoopTickListener(this, &InputService::LateProcess, LateProcessTickPriority))
	{}

	InputService::~InputService()
	{
		Engine::Get()->GetMainLoop()->RemoveTickListener(_processListener);
		Engine::Get()->GetMainLoop()->RemoveTickListener(_lateProcessListener);

		_processListener.reset();
		_lateProcessListener.reset();

		_keyboard.reset();
		_mouse.reset();
	}

	Logging::Logger* InputService::GetLogger() const
	{
		return Engine::Get()->GetLogger();
	}

	void InputService::Start()
	{
		Engine::Get()->GetMainLoop()->AddTickListener(_processListener);
		Engine::Get()->GetMainLoop()->AddTickListener(_lateProcessListener);
	}

	void InputService::Process(double deltaTime)
	{
		_keyboard->ProcessCurrentState();
		_mouse->ProcessCurrentState();
	}

	void InputService::LateProcess(double deltaTime)
	{
		_keyboard->SavePreviousState();
		_mouse->SavePreviousState();
	}
}