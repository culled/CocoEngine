#pragma once

#include "Windowpch.h"
#include <Coco/Core/Core.h>
#include <Coco/Core/Services/EngineService.h>
#include "Window.h"
#include "DisplayInfo.h"

namespace Coco::Windowing
{
	/// @brief Provides windowing services
	class WindowService : public EngineService, public Singleton<WindowService>
	{
		friend Window;

	private:
		std::vector<ManagedRef<Window>> _windows;

	public:
		WindowService(bool dpiAware);
		~WindowService();

		/// @brief Creates a window
		/// @param createParams The parameters to create the window with
		/// @return The created window, or nullptr if the window couldn't be created
		Ref<Window> CreateWindow(const WindowCreateParams& createParams);

		/// @brief Gets the first window created (if one exists) 
		/// @return The first created window, or nullptr if no windows have been created
		Ref<Window> GetMainWindow() const;

		/// @brief Gets a window from its ID
		/// @param windowID The id of the window
		/// @return A pointer to the window, or nullptr if it doesn't exist
		Ref<Window> GetWindow(const WindowID& windowID) const;

		/// @brief Tries to find the visible window that is underneath a given point
		/// @param point The point, in screen coordinates
		/// @param outWindow Will be set to the window if found
		/// @return True if a window was found
		bool GetWindowUnderPoint(const Vector2Int& point, Ref<Window>& outWindow) const;

		/// @brief Gets information for all connected displays
		/// @return Information for all connected displays
		std::vector<DisplayInfo> GetDisplays() const;

		/// @brief Gets a list of all windows currently visible
		/// @return A list of visible windows
		std::vector<Ref<Window>> GetVisibleWindows() const;

	protected:
		/// @brief Called when a window is closed
		/// @param window The window that closed
		void WindowClosed(Window& window);
	};
}