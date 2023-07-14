#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Core/Types/Optional.h>
#include <Coco/Core/Types/Size.h>
#include <Coco/Core/Types/Vector.h>
#include <Coco/Core/Events/Event.h>
#include <Coco/Rendering/Graphics/Resources/GraphicsPresenter.h>
#include "WindowExceptions.h"
#include "WindowTypes.h"

namespace Coco::Windowing
{
	class WindowingService;

	/// @brief Parameters for creating a window
	struct COCOAPI WindowCreateParameters
	{
		/// @brief The title for the window
		string Title;

		/// @brief The initial size for the window, if starting in a windowed state
		SizeInt InitialSize;

		/// @brief If true, the window will be resizable/maximizable
		bool IsResizable = true;

		/// @brief The initial state for the window
		WindowState InitialState = WindowState::Normal;

		/// @brief If true, the window will be shown in fullscreen
		bool IsFullscreen = false;

		/// @brief If provided, the window's top-left corner will be located here
		Optional<Vector2Int> InitialPosition;

		/// @brief If provided, this will be the index of the display that the window will be positioned on
		Optional<int> DisplayIndex;

		WindowCreateParameters(
			const string& title, 
			const SizeInt initialSize, 
			bool isResizable = true, 
			WindowState initialState = WindowState::Normal, 
			bool isFullscreen = false
		) noexcept :
			Title(title), 
			InitialSize(initialSize), 
			IsResizable(isResizable), 
			InitialState(initialState), 
			IsFullscreen(isFullscreen)
		{}
	};

	/// @brief A GUI window
	class COCOAPI Window
	{
	public:
		/// @brief Invoked when the window is trying to close. Setting the bool value to true means the close is cancelled
		Event<Window*, bool&> OnClosing;

		/// @brief Invoked when the window has closed
		Event<Window*> OnClosed;

		/// @brief Invoked when the window is resized
		Event<Window*, const SizeInt&> OnResized;

	protected:
		/// @brief The presenter for the window
		Ref<Rendering::GraphicsPresenter> _presenter;

	protected:
		Window() = default;

	public:
		virtual ~Window() = default;

		Window(const Window& other) = delete;
		Window(Window&& other) = delete;

		Window& operator=(const Window& other) = delete;
		Window& operator=(Window&& other) = delete;

		/// @brief Gets this window's presenter
		/// @return This window's presenter
		Ref<Rendering::GraphicsPresenter> GetPresenter() const noexcept { return _presenter; }

		/// @brief Gets the platform-specific ID for this window
		/// @return The ID for this window
		virtual void* GetID() const noexcept = 0;

		/// @brief Gets the title of this window
		/// @return This window's title
		virtual string GetTitle() const noexcept = 0;

		/// @brief Sets the title of this window
		/// @param title The new title for this window
		virtual void SetTitle(const string& title) = 0;

		/// @brief Gets the size of the window's client area
		/// @return The size of the window's client area
		virtual SizeInt GetSize() const noexcept = 0;

		/// @brief Gets the drawable size of the window's backbuffer
		/// @return The size of the window's backbuffer
		virtual SizeInt GetBackbufferSize() const noexcept = 0;

		/// @brief Shows/restores this window
		virtual void Show() = 0;

		/// @brief Sets the state of this window
		/// @param newState The state for this window
		virtual void SetState(WindowState newState) = 0;

		/// @brief Gets the current state of this window
		/// @return The current state of this window
		virtual WindowState GetState() const noexcept = 0;

		/// @brief Sets if this window should be fullscreen
		/// @param isFullscreen If true, the window will be fullscreen
		virtual void SetIsFullscreen(bool isFullscreen) = 0;

		/// @brief Gets the fullscreen status of this window
		/// @return True if this window is fullscreen
		virtual bool GetIsFullscreen() const = 0;

		/// @brief Gets if this window is visible (shown and not minimized)
		/// @return True if this window is visible
		virtual bool GetIsVisible() const noexcept = 0;

		/// @brief Requests this window to close
		/// @return True if this window will close
		bool Close() noexcept;

	protected:
		/// @brief Called when the window has resized
		void HandleResized();

		/// @brief Sets up the surface for the window presenter
		virtual void SetupPresenterSurface() = 0;
	};
}