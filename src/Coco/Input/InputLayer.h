#pragma once

#include "Mouse.h"
#include "Keyboard.h"

namespace Coco::Input
{
	/// @brief A layer that can consume input events
	struct InputLayer
	{
		virtual ~InputLayer() = default;

		/// @brief Gets the priority of this input layer. Lower priority = earlier in input chain
		/// @return This layer's priority
		virtual int GetPriority() const = 0;

		/// @brief Called when the mouse's state has changed
		/// @param state The mouse state changes
		/// @param mouse The mouse
		/// @return True if the mouse input was consumed
		virtual bool HandleMouseStateChange(const MouseStateChange& state, const Mouse& mouse);

		/// @brief Called when the keyboard's state has changed
		/// @param state The keyboard state changes
		/// @param keyboard The keyboard
		/// @return True if the keyboard input was consumed
		virtual bool HandleKeyboardStateChange(const KeyboardStateChange& state, const Keyboard& keyboard);
	};
}