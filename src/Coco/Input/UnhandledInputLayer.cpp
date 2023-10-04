#include "Inputpch.h"
#include "UnhandledInputLayer.h"

namespace Coco::Input
{
	bool UnhandledInputLayer::HandleMouseStateChange(const MouseStateChange& state)
	{
		return OnMouseStateChanged.Invoke(state);
	}

	bool UnhandledInputLayer::HandleKeyboardStateChange(const KeyboardStateChange& state)
	{
		return OnKeyboardStateChanged.Invoke(state);
	}
}