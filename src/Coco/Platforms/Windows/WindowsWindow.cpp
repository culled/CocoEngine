#include "WindowsWindow.h"
#include "EnginePlatformWindows.h"

#include <Coco/Windowing/WindowingService.h>
#include <Coco/Core/Logging/Logger.h>
#include <Coco/Rendering/Graphics/PresenterSurfaceInitializationInfo.h>

#define CheckWindowHandle() Assert(_handle != NULL)

namespace Coco::Platform::Windows
{
	WindowsWindow::WindowsWindow(Coco::Windowing::WindowCreateParameters& createParameters, Coco::Windowing::WindowingService* windowingService, EnginePlatformWindows* platform) :
		Window(windowingService), _instance(platform->_instance), _size(createParameters.InitialSize)
	{
#if UNICODE || _UNICODE
		std::wstring title = EnginePlatformWindows::StringToWideString(createParameters.Title);
#else
		string title = createParameters.Title;
#endif

		int windowFlags = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		int windowFlagsEx = WS_EX_APPWINDOW;

		if (createParameters.IsResizable)
			windowFlags |= WS_MAXIMIZEBOX | WS_THICKFRAME;

		switch (createParameters.InitialState)
		{
		case Windowing::WindowState::Minimized:
			windowFlags |= WS_MINIMIZE;
			break;
		case Windowing::WindowState::Maximized:
		{
			if(createParameters.IsResizable)
				windowFlags |= WS_MAXIMIZE;
			break;
		}
		default:
			break;
		}

		int x = CW_USEDEFAULT;
		int y = CW_USEDEFAULT;

		// We want the sizing/positioning to be relative to the client space within the window (not the title-bar, etc.)
		// So we need to calculate the border size and offset our position/sizing respectively
		RECT borderRect = {0, 0, 0, 0};
		AdjustWindowRectEx(&borderRect, windowFlags, FALSE, windowFlagsEx);
		
		if (createParameters.InitialPosition.has_value())
		{
			x = createParameters.InitialPosition->X + borderRect.left;
			y = createParameters.InitialPosition->Y + borderRect.top;
		}

		const int width = createParameters.InitialSize.Width + (borderRect.right - borderRect.left);
		const int height = createParameters.InitialSize.Height + (borderRect.bottom - borderRect.top);

		_handle = CreateWindowEx(
			windowFlagsEx,
			EnginePlatformWindows::s_windowClassName,
			title.c_str(),
			windowFlags,
			x,
			y,
			width,
			height,
			NULL, // TODO: parented windows
			NULL, // Menu
			platform->_instance,
			this);

		if (_handle == NULL)
		{
			string error = FormattedString("Failed to create window: {}", GetLastError());
			throw Exception(error.c_str());
		}

		LogTrace(WindowingService->GetLogger(), "Created Windows window");
	}

	WindowsWindow::~WindowsWindow()
	{
		if (_handle != NULL)
		{
			DestroyWindow(_handle);
			_handle = NULL;
		}

		LogTrace(WindowingService->GetLogger(), "Destroyed Windows window");
	}

	void WindowsWindow::Show() noexcept
	{
		CheckWindowHandle();

		ShowWindow(_handle, SW_SHOW);
	}

	void WindowsWindow::Hide() noexcept
	{
		CheckWindowHandle();
	}

	bool WindowsWindow::GetIsVisible() const noexcept
	{
		return IsWindowVisible(_handle);
	}

	void WindowsWindow::SetupPresenterSurface()
	{
		Rendering::PresenterWin32SurfaceInitializationInfo surfaceInitInfo(_handle, _instance);
		Presenter->InitializeSurface(&surfaceInitInfo);
	}

	void WindowsWindow::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_CLOSE:
			Close();
			break;
		case WM_SIZE:
		{
			const UINT width = LOWORD(lParam);
			const UINT height = HIWORD(lParam);
			_size = SizeInt(static_cast<int>(width), static_cast<int>(height));
			HandleResized();
			break;
		}
		default:
			//LogTrace(WindowingService->GetLogger(), FormattedString("Got message {0}", message));
			break;
		}
	}
}