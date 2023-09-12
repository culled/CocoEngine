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
		std::vector<UniqueRef<Window>> _windows;

	public:
		WindowService(bool dpiAware);
		~WindowService();

		/// @brief Creates a window
		/// @param createParams The parameters to create the window with
		/// @return The created window, or nullptr if the window couldn't be created
		Window* CreateWindow(const WindowCreateParams& createParams);

		/// @brief Gets the first window created (if one exists) 
		/// @return The first created window, or nullptr if no windows have been created
		Window* GetMainWindow();

		/// @brief Gets a window from its ID
		/// @param windowID The id of the window
		/// @return A pointer to the window, or nullptr if it doesn't exist
		Window* GetWindow(const WindowID& windowID);

		/// @brief Gets a window from its ID
		/// @param windowID The id of the window
		/// @return A pointer to the window, or nullptr if it doesn't exist
		const Window* GetWindow(const WindowID& windowID) const;

		/// @brief Gets information for all connected displays
		/// @return Information for all connected displays
		std::vector<DisplayInfo> GetDisplays() const;

	protected:
		/// @brief Called when a window is closed
		/// @param window The window that closed
		void WindowClosed(Window& window);
	};
}