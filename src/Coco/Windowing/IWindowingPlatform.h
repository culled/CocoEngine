#pragma once

#include <Coco/Core/API.h>

#include <Coco/Core/Types/Refs/ManagedRef.h>
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
		/// @return The created window
		virtual ManagedRef<::Coco::Windowing::Window> CreatePlatformWindow(const ::Coco::Windowing::WindowCreateParameters& createParameters) = 0;
	};
}