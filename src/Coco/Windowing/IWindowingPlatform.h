#pragma once

#include <Coco/Core/API.h>

#include <Coco/Core/Types/ManagedRef.h>
#include "Window.h"
#include "WindowingService.h"

namespace Coco::Platform
{
	/// @brief An interface that allows EnginePlatforms to create windows
	class COCOAPI IWindowingPlatform
	{
	public:
		/// @brief Creates a platform-specific window
		/// @param createParameters Parameters for creating the window
		/// @param windowingService The service that will manage the window
		/// @return The created window
		virtual ManagedRef<::Coco::Windowing::Window> CreatePlatformWindow(
			::Coco::Windowing::WindowCreateParameters& createParameters, 
			::Coco::Windowing::WindowingService* windowingService) = 0;
	};
}