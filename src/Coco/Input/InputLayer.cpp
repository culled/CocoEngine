#include "Inputpch.h"
#include "InputLayer.h"
#include "InputService.h"

namespace Coco::Input
{
	bool InputLayer::HandleMouseStateChange(const MouseStateChange& state, const Mouse& mouse)
	{
		return false;
	}

	bool InputLayer::HandleKeyboardStateChange(const KeyboardStateChange& state, const Keyboard& keyboard)
	{
		return false;
	}
}