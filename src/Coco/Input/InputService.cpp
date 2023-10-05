#include "Inputpch.h"
#include "InputService.h"

#include <Coco/Core/Engine.h>

namespace Coco::Input
{
	InputService::InputService() :
		_keyboard(CreateUniqueRef<Keyboard>()),
		_mouse(CreateUniqueRef<Mouse>()),
		_processTickHandler(CreateManagedRef<TickListener>(this, &InputService::HandleProcessTick, ProcessTickPriority)),
		_postTickHandler(CreateManagedRef<TickListener>(this, &InputService::HandlePostTick, PostProcessTickPriority)),
		_inputLayers(),
		_inputLayersNeedSorting(false),
		_unhandledInputLayer(CreateManagedRef<UnhandledInputLayer>())
	{
		MainLoop* loop = MainLoop::Get();
		loop->AddListener(_processTickHandler);
		loop->AddListener(_postTickHandler);

		_inputLayers.push_back(_unhandledInputLayer);

		CocoTrace("InputService initialized")
	}

	InputService::~InputService()
	{
		_unhandledInputLayer.Invalidate();

		MainLoop* loop = MainLoop::Get();
		loop->RemoveListener(_processTickHandler);
		loop->RemoveListener(_postTickHandler);

		_mouse.reset();
		_keyboard.reset();

		CocoTrace("InputService shutdown")
	}

	void InputService::RegisterInputLayer(Ref<InputLayer> layer)
	{
		auto it = std::find(_inputLayers.begin(), _inputLayers.end(), layer);

		if (it != _inputLayers.end())
			return;

		_inputLayers.push_back(layer);
		_inputLayersNeedSorting = true;
	}

	void InputService::UnregisterInputLayer(Ref<InputLayer> layer)
	{
		auto it = std::find(_inputLayers.begin(), _inputLayers.end(), layer);

		if (it == _inputLayers.end())
			return;

		_inputLayers.erase(it);
	}

	void InputService::HandleProcessTick(const TickInfo& tickInfo)
	{
		if (_inputLayersNeedSorting)
			SortInputLayers();	

		std::vector<Ref<InputLayer>> tempLayers = _inputLayers;

		for (const MouseStateChange& state : _mouse->GetStateChanges())
		{
			for (uint64 i = 0; i < tempLayers.size(); i++)
			{
				try
				{
					if (tempLayers.at(i)->HandleMouseStateChange(state, *_mouse))
						break;
				}
				catch (const std::exception& ex)
				{
					CocoError("Error handling mouse state changes : {}", ex.what())
				}
			}
		}

		for (const KeyboardStateChange& state : _keyboard->GetStateChanges())
		{
			for (uint64 i = 0; i < tempLayers.size(); i++)
			{
				try
				{
					if (tempLayers.at(i)->HandleKeyboardStateChange(state, *_keyboard))
						break;
				}
				catch (const std::exception& ex)
				{
					CocoError("Error handling keyboard state changes : {}", ex.what())
				}
			}
		}
	}

	void InputService::SortInputLayers()
	{
		auto it = _inputLayers.begin();
		while (it != _inputLayers.end())
		{
			if (it->IsValid())
			{
				it++;
			}
			else
			{
				it = _inputLayers.erase(it);
			}
		}

		std::sort(_inputLayers.begin(), _inputLayers.end(), [](const Ref<InputLayer>& a, const Ref<InputLayer>& b)
			{
				if (!a.IsValid())
					return false;

				if (!b.IsValid())
					return true;

				return a->GetPriority() < b->GetPriority();
			});

		_inputLayersNeedSorting = false;
	}

	void InputService::HandlePostTick(const TickInfo& tickInfo)
	{
		_keyboard->SavePreviousState();
		_mouse->SavePreviousState();
	}
}