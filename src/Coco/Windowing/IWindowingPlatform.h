#pragma once

#include <Coco/Core/Core.h>
#include "Window.h"
#include "WindowingService.h"

namespace Coco::Platform
{
	/// <summary>
	/// An interface that allows EnginePlatforms to create windows
	/// </summary>
	class COCOAPI IWindowingPlatform
	{
	public:
		/// <summary>
		/// Creates a platform-specific window
		/// </summary>
		/// <param name="createParameters">Parameters for creating the window</param>
		/// <param name="windowingService">The service that will manage the window</param>
		/// <returns>The created window</returns>
		virtual Managed<::Coco::Windowing::Window> CreatePlatformWindow(
			::Coco::Windowing::WindowCreateParameters& createParameters, 
			::Coco::Windowing::WindowingService* windowingService) = 0;
	};
}