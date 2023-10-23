#include "Inputpch.h"
#include "UnhandledInputLayer.h"

namespace Coco::Input
{
	bool UnhandledInputLayer::HandleMouseStateChange(const MouseStateChange& state, const Mouse& mouse)
	{
		return OnMouseStateChanged.Invoke(state);
	}

	bool UnhandledInputLayer::HandleKeyboardStateChange(const KeyboardStateChange& state, const Keyboard& keyboard)
	{
		return OnKeyboardStateChanged.Invoke(state);
	}
}