#pragma once

#include "Windowpch.h"
#include <Coco/Core/Types/Refs.h>
#include "Window.h"
#include "DisplayInfo.h"

namespace Coco::Windowing
{
	/// @brief Allows an EnginePlatform to support GUI windows
	class WindowingPlatform
	{
	public:
		/// @brief Gets if this platform supports more than one window open at once
		/// @return If this platform supports multiple windows
		virtual bool SupportsMultipleWindows() const = 0;

		/// @brief Creates a platform-specific window
		/// @param createParams The parameters to create the window with
		/// @return The created window
		virtual UniqueRef<Window> CreatePlatformWindow(const WindowCreateParams& createParams) = 0;

		/// @brief Gets information for all connected displays
		/// @return Information for all connected displays
		virtual std::vector<DisplayInfo> GetDisplays() const = 0;

		/// @brief Sets the dpi-awareness mode of this application.
		/// NOTE: this should be set before any windows are created
		/// @param dpiAware If true, windows will be scaled based on DPI
		virtual void SetDPIAwareMode(bool dpiAware) = 0;
	};
}