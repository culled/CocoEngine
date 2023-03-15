#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/Optional.h>
#include <Coco/Core/Types/Size.h>
#include <Coco/Core/Types/Vector.h>
#include <Coco/Core/Events/Event.h>
#include <Coco/Rendering/Graphics/GraphicsPresenter.h>

#include "WindowTypes.h"

namespace Coco::Windowing
{
	class WindowingService;

	/// <summary>
	/// Parameters for creating a window
	/// </summary>
	struct COCOAPI WindowCreateParameters
	{
		/// <summary>
		/// The title for the window
		/// </summary>
		string Title;

		/// <summary>
		/// The initial size for the window, if starting in a windowed state
		/// </summary>
		SizeInt InitialSize;

		/// <summary>
		/// If true, the window will be resizable/maximizable
		/// </summary>
		bool IsResizable = true;

		/// <summary>
		/// If provided, the window's top-left corner will be located here
		/// </summary>
		Optional<Vector2Int> InitialPosition;

		/// <summary>
		/// The initial state for the window
		/// </summary>
		WindowState InitialState = WindowState::Windowed;

		WindowCreateParameters(const string& title, const SizeInt initialSize, bool isResizable = true, WindowState initialState = WindowState::Windowed) noexcept :
			Title(title), InitialSize(initialSize), IsResizable(isResizable), InitialState(initialState)
		{}
	};

	/// <summary>
	/// A GUI window
	/// </summary>
	class COCOAPI Window
	{
	protected:
		/// <summary>
		/// The presenter for the window
		/// </summary>
		Managed<Rendering::GraphicsPresenter> Presenter;

		/// <summary>
		/// The windowing service
		/// </summary>
		WindowingService* WindowingService;

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

		/// <summary>
		/// Invoked when the window is resized
		/// </summary>
		Event<Window*, const SizeInt&> OnResized;

	protected:
		Window(Windowing::WindowingService* windowingService);

	public:
		virtual ~Window();

		Window() = delete;
		Window(const Window& other) = delete;
		Window(Window&& other) = delete;

		Window& operator=(const Window& other) = delete;
		Window& operator=(Window&& other) = delete;

		/// <summary>
		/// Gets this window's presenter
		/// </summary>
		/// <returns>This window's presenter</returns>
		Rendering::GraphicsPresenter* GetPresenter() const noexcept { return Presenter.get(); }

		/// <summary>
		/// Gets the platform-specific ID for this window
		/// </summary>
		/// <returns>The ID for this window</returns>
		virtual void* GetID() const noexcept = 0;

		/// <summary>
		/// Gets the size of the window's client area
		/// </summary>
		/// <returns>The size of the window's client area</returns>
		virtual SizeInt GetSize() const noexcept = 0;

		/// <summary>
		/// Shows this window
		/// </summary>
		virtual void Show() = 0;

		/// <summary>
		/// Hides this window
		/// </summary>
		virtual void Hide() = 0;

		/// <summary>
		/// Gets if this window is visible (shown and not minimized)
		/// </summary>
		/// <returns>True if this window is visible</returns>
		virtual bool GetIsVisible() const = 0;

		/// <summary>
		/// Requests this window to close
		/// </summary>
		/// <returns>True if this window will close</returns>
		bool Close() noexcept;

	protected:
		/// <summary>
		/// Called when the window has resized
		/// </summary>
		void HandleResized();

		/// <summary>
		/// Sets up the surface for the window presenter
		/// </summary>
		virtual void SetupPresenterSurface() = 0;

		/// <summary>
		/// Gets the drawable size of the window's backbuffer
		/// </summary>
		/// <returns>The size of the window's backbuffer</returns>
		virtual SizeInt GetBackbufferSize() const noexcept = 0;
	};
}