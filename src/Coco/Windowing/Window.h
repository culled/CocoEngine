#pragma once

#include "Windowpch.h"

#include <Coco/Core/Math/Math.h>
#include <Coco/Core/Events/Event.h>
#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/Rect.h>

#include "WindowTypes.h"

namespace Coco::Rendering
{
	class GraphicsPresenter;
	struct GraphicsPresenterSurface;
}

namespace Coco::Windowing
{
	/// @brief A GUI window
	class Window
	{
	public:
		/// @brief An invalid window ID
		static const WindowID InvalidID;

		/// @brief The default DPI
		static const uint16 DefaultDPI;

		/// @brief The ID of this window
		const WindowID ID;

		/// @brief Invoked when this window is requested to close
		Event<bool&> OnClosing;

		/// @brief Invoked when this window is closing
		Event<> OnClosed;

		/// @brief Invoked when this window's size changes
		Event<const SizeInt&> OnResized;

		/// @brief Invoked when this window's position changes
		Event<const Vector2Int&> OnPositionChanged;

		/// @brief Invoked when this window's dpi changes (like being moved to a different monitor)
		Event<uint16> OnDPIChanged;

		/// @brief Invoked when this window had become focused/unfocused
		Event<bool> OnFocusChanged;

	protected:
		WindowID _parentID;
		Ref<Rendering::GraphicsPresenter> _presenter;

	private:
		static std::atomic<WindowID> _id;

	protected:
		Window(const WindowCreateParams& createParams);

	public:
		virtual ~Window();

		/// @brief Shows this window
		virtual void Show() = 0;

		/// @brief Sets this window's title
		/// @param title The title
		virtual void SetTitle(const char* title) = 0;

		/// @brief Gets this window's title
		/// @return The title
		virtual const char* GetTitle() const = 0;

		/// @brief Sets this window's fullscreen state
		/// @param fullscreen If true, this window will attempt to go fullscreen
		virtual void SetIsFullscreen(bool fullscreen) = 0;

		/// @brief Gets this window's fullscreen state
		/// @return If this window is fullscreen
		virtual bool IsFullscreen() const = 0;

		/// @brief Sets the top-left position of this window
		/// @param position The position
		/// @param clientAreaPosition If true, the top-left corner of the client area will be set to the position
		/// @param relativeToParent If true, the position will be relative to this window's parent's top-left corner
		virtual void SetPosition(const Vector2Int& position, bool clientAreaPosition = false, bool relativeToParent = true) = 0;

		/// @brief Gets the top-left position of this window
		/// @param clientAreaPosition If true, the top-left corner of the client area will be returned
		/// @param relativeToParent If true, the position will be relative to this window's parent's top-left corner
		/// @return The position
		virtual Vector2Int GetPosition(bool clientAreaPosition = false, bool relativeToParent = true) const = 0;

		/// @brief Sets the client-area size of this window
		/// @param size The size of the client-area
		virtual void SetClientAreaSize(const SizeInt& size) = 0;

		/// @brief Gets the client-area size of this window
		/// @return The client-area size
		virtual SizeInt GetClientAreaSize() const = 0;

		/// @brief Sets the size of the entire window
		/// @param windowSize The size of the window
		virtual void SetSize(const SizeInt& windowSize) = 0;

		/// @brief Gets the total size of this window
		/// @return The total size
		virtual SizeInt GetSize() const = 0;

		/// @brief Gets the DPI of the screen this window is currently on
		/// @return The current DPI
		virtual uint16 GetDPI() const = 0;

		/// @brief Sets the state of this window
		/// @param state The state
		virtual void SetState(WindowState state) = 0;

		/// @brief Gets the state of this window
		/// @return The state of this window
		virtual WindowState GetState() const = 0;

		/// @brief Focuses this window
		virtual void Focus() = 0;
		
		/// @brief Determines if this window has focus
		/// @return True if this window has focus
		virtual bool HasFocus() const = 0;

		/// @brief Determines if this window is visible
		/// @return True if this window is visible
		virtual bool IsVisible() const = 0;

		/// @brief Sets the cursor visiblity once it's over this window
		virtual void SetCursorVisibility(bool isVisible) = 0;

		/// @brief Gets the cursor visibility of this window
		/// @return True if the cursor is visible when it is over this window
		virtual bool GetCursorVisibility() const = 0;

		/// @brief Gets this window's GraphicsPresenter
		/// @return This window's GraphicsPresenter
		Ref<Rendering::GraphicsPresenter> GetPresenter() const { return _presenter; }

		/// @brief Closes this window
		void Close();

		/// @brief Determines if this window is parented
		/// @return True if this window has a parent
		bool HasParent() const;

		/// @brief Gets the ID of this window's parent
		/// @return The ID of the parent window, or Window::InvalidID if this window has no parent
		WindowID GetParentID() const;

		/// @brief Gets a rectangle that represents the position and size of this window
		/// @param clientArea If true, the rectangle will only be for the client area
		/// @param relativeToParent If true, the rectangle will be relative to this window's parent, if it has one
		/// @return A rectangle that this window convers
		RectInt GetRect(bool clientArea, bool relativeToParent = true) const;

	protected:
		/// @brief Creates a surface for this window to render to
		/// @return The surface
		virtual SharedRef<Rendering::GraphicsPresenterSurface> CreateSurface() = 0;

		/// @brief Gets the parent window from the window service, if one exists
		/// @return A pointer to the parent window, or nullptr if the parent could not be found or this window has no parent
		Ref<Window> GetParentWindow() const;

		/// @brief Called when this window is resized
		void HandleResized();
	};
}