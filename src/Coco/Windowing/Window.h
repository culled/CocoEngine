#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/Optional.h>
#include <Coco/Core/Types/Size.h>
#include <Coco/Core/Types/Vector.h>
#include <Coco/Core/Events/Event.h>

#include "WindowTypes.h"

namespace Coco::Windowing
{
	class WindowingService;

	/// <summary>
	/// Parameters for creating a window
	/// </summary>
	struct COCOAPI WindowCreateParameters
	{
		string Title;
		SizeInt InitialSize;
		bool IsResizable = true;
		Optional<Vector2Int> InitialPosition;
		WindowState InitialState = WindowState::Windowed;

		WindowCreateParameters(const string& title, const SizeInt initialSize, bool isResizable = true, WindowState initialState = WindowState::Windowed) :
			Title(title), InitialSize(initialSize), IsResizable(isResizable), InitialState(initialState)
		{}
	};

	/// <summary>
	/// A GUI window
	/// </summary>
	class COCOAPI Window
	{
	public:
		/// <summary>
		/// Invoked when the window is trying to close.
		/// Setting the bool value to true means the close is cancelled
		/// </summary>
		Event<Window*, bool&> OnClosing;

		/// <summary>
		/// Invoked when the window has closed
		/// </summary>
		Event<Window*> OnClosed;

	protected:
		WindowingService* WindowingService;

	protected:
		Window(Windowing::WindowingService* windowingService);

	public:
		virtual ~Window();

		/// <summary>
		/// Gets the platform-specific ID for this window
		/// </summary>
		/// <returns>The ID for this window</returns>
		virtual void* GetID() const = 0;

		/// <summary>
		/// Shows this window
		/// </summary>
		virtual void Show() = 0;

		/// <summary>
		/// Hides this window
		/// </summary>
		virtual void Hide() = 0;

		/// <summary>
		/// Requests this window to close
		/// </summary>
		/// <returns>True if this window will close</returns>
		bool Close();
	};
}