#include "WindowsWindow.h"

#include "EnginePlatformWindows.h"

#include <Coco/Core/Logging/Logger.h>
#include <Coco/Windowing/WindowingService.h>
#include <Coco/Rendering/Graphics/Resources/PresenterSurfaceInitializationInfo.h>
#include <Coco/Core/Services/EngineServiceManager.h>
#include <Coco/Rendering/RenderingService.h>

#define CheckWindowHandle() Assert(_handle != NULL)

namespace Coco::Platform::Windows
{
	WindowsWindow::WindowsWindow(const Coco::Windowing::WindowCreateParameters& createParameters) :
		Window(),
		_canResize(createParameters.IsResizable),
		_restorePlacement{ sizeof(WINDOWPLACEMENT) }
	{
		if (Rendering::RenderingService::Get() == nullptr)
			throw Windowing::WindowCreateException("Failed to find an active RenderingService");

		_presenter = Rendering::RenderingService::Get()->GetPlatform()->CreatePresenter(createParameters.Title);

		_instance = EnginePlatformWindows::Get()->_instance;
		_size = createParameters.InitialSize;

#if UNICODE || _UNICODE
		std::wstring title = StringToWideString(createParameters.Title);
#else
		string title = createParameters.Title;
#endif

		int windowFlags = GetWindowFlags();
		int windowFlagsEx = WS_EX_APPWINDOW;

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

		if (createParameters.IsFullscreen)
			windowFlags &= ~WS_OVERLAPPEDWINDOW;

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

		if (createParameters.DisplayIndex.has_value())
		{
			auto displays = Windowing::WindowingService::Get()->GetDisplays();
			
			if (createParameters.DisplayIndex.value() >= 0 || createParameters.DisplayIndex.value() < displays.Count())
			{
				const Windowing::DisplayInfo& display = displays[createParameters.DisplayIndex.value()];

				if (x == CW_USEDEFAULT || y == CW_USEDEFAULT)
				{
					x = display.Offset.X + (display.Resolution.Width - width) / 2;
					y = display.Offset.Y + (display.Resolution.Height - height) / 2;
				}
				else
				{
					x = display.Offset.X + createParameters.InitialPosition->X;
					y = display.Offset.Y + createParameters.InitialPosition->Y;
				}
			}
		}

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
			_instance,
			this);

		if (_handle == NULL)
			throw Windowing::WindowCreateException(FormattedString("Failed to create window: {}", GetLastError()));

		LogTrace(Windowing::WindowingService::Get()->GetLogger(), "Created Windows window");
	}

	WindowsWindow::~WindowsWindow()
	{
		if (_handle != NULL)
		{
			DestroyWindow(_handle);
			_handle = NULL;
		}

		LogTrace(Windowing::WindowingService::Get()->GetLogger(), "Destroyed Windows window");
	}

	string WindowsWindow::GetTitle() const noexcept
	{
		CheckWindowHandle();

		int titleLength = GetWindowTextLength(_handle) + 1;
		LPTSTR rawTitle = nullptr;
		string title;

		try
		{
			rawTitle = new TCHAR[titleLength];

			if (rawTitle != NULL)
			{
				int count = GetWindowText(_handle, rawTitle, titleLength);

#ifdef UNICODE
				title = EnginePlatformWindows::WideStringToString(rawTitle);
#else
				title = string((LPSTR)rawTitle, count);
#endif
			}
		}
		catch (const Exception& e)
		{
			LogError(Windowing::WindowingService::Get()->GetLogger(), FormattedString("Failed to get window title: {}", e.what()));
		}

		if (rawTitle != NULL)
			delete[] rawTitle;

		return title;
	}

	void WindowsWindow::SetTitle(const string& title)
	{
		CheckWindowHandle();

#ifdef UNICODE
		std::wstring titleStr = StringToWideString(title);
#else
		std::string titleStr = title;
#endif // UNICODE

		SetWindowText(_handle, titleStr.c_str());
	}

	void WindowsWindow::Show() noexcept
	{
		CheckWindowHandle();

		ShowWindow(_handle, SW_SHOW);
		UpdateFullscreen(GetIsFullscreen());
	}

	void WindowsWindow::SetState(Windowing::WindowState newState)
	{
		if (GetState() == newState)
			return;

		UpdateState(newState);
	}

	Windowing::WindowState WindowsWindow::GetState() const noexcept
	{
		CheckWindowHandle();

		DWORD windowStyle = ::GetWindowLong(_handle, GWL_STYLE);

		if (windowStyle & WS_MINIMIZE)
			return Windowing::WindowState::Minimized;
		else if (windowStyle & WS_MAXIMIZE)
			return Windowing::WindowState::Maximized;
		
		return Windowing::WindowState::Normal;
	}

	void WindowsWindow::SetIsFullscreen(bool isFullscreen)
	{
		if (GetIsFullscreen() == isFullscreen)
			return;

		UpdateFullscreen(isFullscreen);
	}

	bool WindowsWindow::GetIsFullscreen() const
	{
		CheckWindowHandle();

		DWORD windowStyle = ::GetWindowLong(_handle, GWL_STYLE);

		return (windowStyle & GetWindowFlags()) != GetWindowFlags();
	}

	bool WindowsWindow::GetIsVisible() const noexcept
	{
		return IsWindowVisible(_handle) && _size.Width > 0 && _size.Height > 0;
	}

	void WindowsWindow::SetupPresenterSurface()
	{
		_presenter->InitializeSurface(Rendering::PresenterWin32SurfaceInitializationInfo(_handle, _instance));
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

			if (_size.Width == 0 && _size.Height == 0 && width > 0 && height > 0)
				UpdateFullscreen(GetIsFullscreen());

			_size = SizeInt(static_cast<int>(width), static_cast<int>(height));
			HandleResized();
			break;
		}
		default:
			//LogTrace(WindowingService->GetLogger(), FormattedString("Got message {0}", message));
			break;
		}
	}

	DWORD WindowsWindow::GetWindowFlags() const
	{
		DWORD flags = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

		if (_canResize)
			flags |= WS_THICKFRAME | WS_MAXIMIZEBOX;

		return flags;
	}

	void WindowsWindow::UpdateState(Windowing::WindowState newState)
	{
		CheckWindowHandle();

		switch (newState)
		{
		case Windowing::WindowState::Minimized:
			ShowWindow(_handle, SW_MINIMIZE);
			break;
		case Windowing::WindowState::Normal:
			if(!GetIsFullscreen())
				ShowWindow(_handle, SW_RESTORE);
			break;
		case Windowing::WindowState::Maximized:
			if (!GetIsFullscreen() && _canResize)
				ShowWindow(_handle, SW_MAXIMIZE);
			break;
		default: 
			break;
		}
	}

	void WindowsWindow::UpdateFullscreen(bool isFullscreen)
	{
		CheckWindowHandle();

		// https://devblogs.microsoft.com/oldnewthing/20100412-00/?p=14353
		
		// Restore the window frame if we're not fullscreen anymore
		if (!isFullscreen && GetIsFullscreen())
		{
			DWORD windowStyle = ::GetWindowLong(_handle, GWL_STYLE);
			::SetWindowLong(_handle, GWL_STYLE, windowStyle | GetWindowFlags());
			::SetWindowPlacement(_handle, &_restorePlacement);
			::SetWindowPos(_handle,
				NULL,
				0, 0,
				0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		} 
		else if (isFullscreen)
		{
			MONITORINFO mi{ sizeof(MONITORINFO) };
			DWORD windowStyle = ::GetWindowLong(_handle, GWL_STYLE);

			if (::GetWindowPlacement(_handle, &_restorePlacement) && ::GetMonitorInfo(::MonitorFromWindow(_handle, MONITOR_DEFAULTTOPRIMARY), &mi))
			{
				::SetWindowLong(_handle, GWL_STYLE, windowStyle & ~WS_OVERLAPPEDWINDOW);
				::SetWindowPos(_handle,
					HWND_TOP,
					mi.rcMonitor.left,
					mi.rcMonitor.top,
					mi.rcMonitor.right - mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top,
					SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
			}
		}
	}
}