#include "Inputpch.h"
#include "InputLayer.h"
#include "InputService.h"

namespace Coco::Input
{
	bool InputLayer::HandleMouseStateChange(const MouseStateChange& state)
	{
		return false;
	}

	bool InputLayer::HandleKeyboardStateChange(const KeyboardStateChange& state)
	{
		return false;
	}
}