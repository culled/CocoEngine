#pragma once

#include "Windowpch.h"
#include <Coco/Core/Types/Size.h>
#include <Coco/Core/Types/Vector.h>

namespace Coco::Windowing
{
	/// @brief States that a window can be in
	enum class WindowState
	{
		Default,
		Minimized,
		Maximized
	};

	/// @brief A window ID
	using WindowID = uint32;

	/// @brief Parameters for creating a window
	struct WindowCreateParams
	{
		/// @brief The initial title
		const char* Title;

		/// @brief The initial client area size
		SizeInt InitialSize;

		/// @brief If true, the window can be resized
		bool CanResize;

		/// @brief The initial state for the window to start in
		WindowState InitialState;

		/// @brief If true, the window will start in fullscreen
		bool IsFullscreen;

		/// @brief If given, the window's top-left corner will be at the given position relative to its parent (if it has one).
		/// NOTE: this will be relative to the top-left corner of the screen if a DisplayIndex is given as well
		std::optional<Vector2Int> InitialPosition;

		/// @brief If given, will be the parent of the created window
		WindowID ParentWindow;

		/// @brief If given, sets the index of the display to show the window on
		std::optional<uint8> DisplayIndex;

		/// @brief If true, the window will not include any decorations (title bar, buttons, frame, etc.) 
		bool WithoutDecoration;

		WindowCreateParams(const char* title, const SizeInt& initialSize);
	};
}