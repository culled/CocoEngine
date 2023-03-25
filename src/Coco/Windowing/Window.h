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

		/// @brief If provided, the window's top-left corner will be located here
		Optional<Vector2Int> InitialPosition;

		/// @brief The initial state for the window
		WindowState InitialState = WindowState::Windowed;

		WindowCreateParameters(const string& title, const SizeInt initialSize, bool isResizable = true, WindowState initialState = WindowState::Windowed) noexcept :
			Title(title), InitialSize(initialSize), IsResizable(isResizable), InitialState(initialState)
		{}
	};

	/// @brief A GUI window
	class COCOAPI Window
	{
	public:
		/// @brief The windowing service
		WindowingService* const WindowingService;

	protected:
		/// @brief The presenter for the window
		WeakManagedRef<Rendering::GraphicsPresenter> Presenter;

	public:
		/// @brief Invoked when the window is trying to close. Setting the bool value to true means the close is cancelled
		Event<Window*, bool&> OnClosing;

		/// @brief Invoked when the window has closed
		Event<Window*> OnClosed;

		/// @brief Invoked when the window is resized
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

		/// @brief Gets this window's presenter
		/// @return This window's presenter
		WeakManagedRef<Rendering::GraphicsPresenter> GetPresenter() const noexcept { return Presenter; }

		/// @brief Gets the platform-specific ID for this window
		/// @return The ID for this window
		virtual void* GetID() const noexcept = 0;

		/// @brief Gets the size of the window's client area
		/// @return The size of the window's client area
		virtual SizeInt GetSize() const noexcept = 0;

		/// @brief Gets the drawable size of the window's backbuffer
		/// @return The size of the window's backbuffer
		virtual SizeInt GetBackbufferSize() const noexcept = 0;

		/// @brief Shows/restores this window
		virtual void Show() = 0;

		/// @brief Minimizes this window
		virtual void Minimize() = 0;

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