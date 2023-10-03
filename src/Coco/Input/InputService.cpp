#include "Inputpch.h"
#include "InputService.h"

#include <Coco/Core/Engine.h>

namespace Coco::Input
{
	InputService::InputService() :
		_keyboard(CreateUniqueRef<Keyboard>()),
		_mouse(CreateUniqueRef<Mouse>()),
		_preTickHandler(CreateManagedRef<TickListener>(this, &InputService::HandlePreTick, PreProcessTickPriority)),
		_postTickHandler(CreateManagedRef<TickListener>(this, &InputService::HandlePostTick, PostProcessTickPriority))
	{
		MainLoop* loop = MainLoop::Get();
		loop->AddListener(_preTickHandler);
		loop->AddListener(_postTickHandler);

		CocoTrace("InputService initialized")
	}

	InputService::~InputService()
	{
		MainLoop* loop = MainLoop::Get();
		loop->RemoveListener(_preTickHandler);
		loop->RemoveListener(_postTickHandler);

		_mouse.reset();
		_keyboard.reset();

		CocoTrace("InputService shutdown")
	}

	void InputService::HandlePreTick(const TickInfo& tickInfo)
	{
		_keyboard->ProcessCurrentState();
		_mouse->ProcessCurrentState();
	}

	void InputService::HandlePostTick(const TickInfo& tickInfo)
	{
		_keyboard->SavePreviousState();
		_mouse->SavePreviousState();
	}
}