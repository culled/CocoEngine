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

	/// @brief Modes for a confining a cursor to a window
	enum class CursorConfineMode
	{
		/// @brief The cursor is not confined
		None,
		
		/// @brief The cursor cannot move outside of the window's client area
		ClientArea,

		/// @brief The cursor is locked to the window's center
		LockedCenter,

		/// @brief The cursor is locked where it currently is
		LockedInPlace,
	};

	enum class WindowStyleFlags
	{
		None = 0,

		/// @brief The window will not include any decorations (title bar, buttons, frame, etc.) 
		NoDecoration = 1 << 0,

		/// @brief This window will not show up in the toolbar of desktop window managers
		NoTaskbarIcon = 1 << 1,

		/// @brief The window will be placed above all other non-topmost windows and stay above them
		TopMost = 1 << 2
	};

	constexpr WindowStyleFlags operator|(const WindowStyleFlags& a, const WindowStyleFlags& b)
	{
		return static_cast<WindowStyleFlags>(static_cast<int>(a) | static_cast<int>(b));
	}

	constexpr WindowStyleFlags operator&(const WindowStyleFlags& a, const WindowStyleFlags& b)
	{
		return static_cast<WindowStyleFlags>(static_cast<int>(a) & static_cast<int>(b));
	}

	constexpr void operator |=(WindowStyleFlags& a, const WindowStyleFlags& b)
	{
		a = a | b;
	}

	constexpr void operator &=(WindowStyleFlags& a, const WindowStyleFlags& b)
	{
		a = a & b;
	}

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

		/// @brief If true, the window will grab focus when it is shown
		bool FocusOnShow;

		/// @brief Styling flags
		WindowStyleFlags StyleFlags;

		WindowCreateParams(const char* title, const SizeInt& initialSize);
	};
}