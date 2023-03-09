#pragma once

#include <Coco/Core/Core.h>

namespace Coco::Rendering
{
	/// <summary>
	/// Generic platform-specific surface initialization info
	/// </summary>
	struct COCOAPI PresenterSurfaceInitializationInfo
	{
	protected:
		PresenterSurfaceInitializationInfo() = default;

	public:
		virtual ~PresenterSurfaceInitializationInfo() = default;
	};

	/// <summary>
	/// Windows surface initialization info
	/// </summary>
	struct COCOAPI PresenterWin32SurfaceInitializationInfo : PresenterSurfaceInitializationInfo
	{
		/// <summary>
		/// The handle to the window (HWND) for the surface
		/// </summary>
		void* HWindow;

		/// <summary>
		/// The handle to the HINSTANCE
		/// </summary>
		void* HInstance;

		PresenterWin32SurfaceInitializationInfo(void* hWnd, void* hInstance) : HWindow(hWnd), HInstance(hInstance) {}
		virtual ~PresenterWin32SurfaceInitializationInfo() override = default;
	};
}