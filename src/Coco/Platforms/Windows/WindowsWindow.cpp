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
		Window(createParameters.Parent),
		_canResize(createParameters.IsResizable),
		_restorePlacement{ sizeof(WINDOWPLACEMENT) }
	{
		if (Rendering::RenderingService::Get() == nullptr)
			throw Windowing::WindowCreateException("Failed to find an active RenderingService");

		_presenter = Rendering::RenderingService::Get()->GetPlatform()->CreatePresenter(createParameters.Title);

		_instance = EnginePlatformWindows::Get()->_instance;

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

		// We want the sizing to be relative to the client space within the window (not the title-bar, etc.)
		// So we need to calculate the border size and offset our position/sizing respectively
		RECT borderRect = {0, 0, 0, 0};
		AdjustWindowRectEx(&borderRect, windowFlags, FALSE, windowFlagsEx);
		
		if (createParameters.InitialPosition.has_value())
		{
			x = createParameters.InitialPosition->X;
			y = createParameters.InitialPosition->Y;
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

		HWND parentWindowHandle = NULL;

		if (createParameters.Parent.has_value())
		{
			const WindowsWindow* parentWindow = static_cast<const WindowsWindow*>(createParameters.Parent->Get());

			parentWindowHandle = parentWindow->_handle;

			if (x != CW_USEDEFAULT && y != CW_USEDEFAULT)
			{
				Vector2Int parentPos = parentWindow->GetPosition();

				x += parentPos.X;
				y += parentPos.Y;
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
			parentWindowHandle,
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

	void WindowsWindow::SetSize(const SizeInt& size)
	{
		CheckWindowHandle();

		int windowFlags = ::GetWindowLong(_handle, GWL_STYLE);
		int windowFlagsEx = ::GetWindowLong(_handle, GWL_EXSTYLE);

		// We want the sizing to be relative to the client space within the window (not the title-bar, etc.)
		// So we need to calculate the border size and offset our position/sizing respectively
		RECT borderRect = { 0, 0, 0, 0 };
		AdjustWindowRectEx(&borderRect, windowFlags, FALSE, windowFlagsEx);

		::SetWindowPos(
			_handle, 
			NULL, 
			0, 0, 
			size.Width + (borderRect.right - borderRect.left), size.Height + (borderRect.bottom - borderRect.top),
			SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER
		);
	}

	SizeInt WindowsWindow::GetSize() const noexcept
	{
		CheckWindowHandle();

		RECT rect;
		::GetClientRect(_handle, &rect);

		return SizeInt(rect.right, rect.bottom);
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
		SizeInt size = GetSize();

		return IsWindowVisible(_handle) && size.Width > 0 && size.Height > 0;
	}

	void WindowsWindow::SetupPresenterSurface()
	{
		_presenter->InitializeSurface(Rendering::PresenterWin32SurfaceInitializationInfo(_handle, _instance));
	}

	void WindowsWindow::SetPosition(const Vector2Int& position)
	{
		CheckWindowHandle();

		Vector2Int pos = position;

		if (_parent.IsValid())
		{
			Vector2Int parentPos = _parent->GetPosition();
			pos += parentPos;
		}

		::SetWindowPos(_handle, NULL, pos.X, pos.Y, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER);
	}

	Vector2Int WindowsWindow::GetPosition() const
	{
		CheckWindowHandle();

		RECT rect;
		::GetWindowRect(_handle, &rect);

		Vector2Int pos = Vector2Int(rect.left, rect.top);

		if (_parent.IsValid())
		{
			Vector2Int parentPos = _parent->GetPosition();
			pos -= parentPos;
		}

		return pos;
	}

	void WindowsWindow::Focus()
	{
		CheckWindowHandle();

		::SetActiveWindow(_handle);
	}

	bool WindowsWindow::HasFocus() const
	{
		CheckWindowHandle();

		return ::GetActiveWindow() == _handle;
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
			SizeInt size = GetSize();

			if (size.Width == 0 && size.Height == 0 && width > 0 && height > 0)
				UpdateFullscreen(GetIsFullscreen());

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