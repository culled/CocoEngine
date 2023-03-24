#pragma once

#include <Coco/Core/API.h>

namespace Coco::Rendering
{
	/// @brief Generic platform-specific surface initialization info
	struct COCOAPI PresenterSurfaceInitializationInfo
	{
	protected:
		PresenterSurfaceInitializationInfo() = default;

	public:
		virtual ~PresenterSurfaceInitializationInfo() = default;
	};

	/// @brief Windows surface initialization info
	struct COCOAPI PresenterWin32SurfaceInitializationInfo : PresenterSurfaceInitializationInfo
	{
		/// @brief The handle to the window (HWND) for the surface
		void* HWindow;

		/// @brief The handle to the HINSTANCE
		void* HInstance;

		PresenterWin32SurfaceInitializationInfo(void* hWnd, void* hInstance) noexcept : HWindow(hWnd), HInstance(hInstance) {}
		~PresenterWin32SurfaceInitializationInfo() final = default;
	};
}