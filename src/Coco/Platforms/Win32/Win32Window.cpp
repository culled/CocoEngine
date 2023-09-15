#include "CPWinpch.h"
#include "Win32Window.h"
#include "Win32EnginePlatform.h"
#include <Coco/Core/Engine.h>

#pragma push_macro("GetFreeSpace")
#undef GetFreeSpace
#include <Coco/Rendering/RenderService.h>
#include <Coco/Rendering/Graphics/GraphicsPresenter.h>
#pragma pop_macro("GetFreeSpace")

#pragma push_macro("CreateWindow")
#undef CreateWindow
#include <Coco/Windowing/WindowService.h>
#pragma pop_macro("CreateWindow")

#ifdef _DEBUG
#define CheckWindowHandle() if(!_handle) { throw std::exception("Win32 window handle is null!"); }
#else
#define CheckWindowHandle()
#endif

namespace Coco::Platforms::Win32
{
	Win32Window::Win32Window(const Windowing::WindowCreateParams& createParams) :
		Window(createParams),
		_title(createParams.Title),
		_canResize(createParams.CanResize),
		_isFullscreen(createParams.IsFullscreen),
		_restorePlacement{sizeof(WINDOWPLACEMENT)}
	{
		DWORD windowFlags = GetWindowFlags(_canResize, _isFullscreen);
		DWORD windowFlagsEx = WS_EX_APPWINDOW;

		// Add flags based on initial state
		switch (createParams.InitialState)
		{
		case Windowing::WindowState::Minimized:
			windowFlags |= WS_MINIMIZE;
			break;
		case Windowing::WindowState::Maximized:
		{
			if (_canResize)
				windowFlags |= WS_MAXIMIZE;
			break;
		}
		default: break;
		}

		Vector2Int pos(CW_USEDEFAULT, CW_USEDEFAULT);

		if (createParams.InitialPosition.has_value())
		{
			pos = createParams.InitialPosition.value();
		}

		SizeInt size = GetAdjustedWindowSize(createParams.InitialSize, windowFlags, windowFlagsEx);

		// Set position relative to initial display (if given)
		if (createParams.DisplayIndex.has_value())
		{
			Windowing::WindowService* windowService = Windowing::WindowService::Get();
			std::vector<DisplayInfo> displays = windowService->GetDisplays();

			if (createParams.DisplayIndex.value() < displays.size())
			{
				const DisplayInfo& display = displays.at(createParams.DisplayIndex.value());

				if (pos.X == CW_USEDEFAULT || pos.X == CW_USEDEFAULT)
				{
					pos = display.Offset + Vector2Int((display.Resolution.Width - size.Width) / 2, (display.Resolution.Height - size.Height) / 2);
				}
				else
				{
					pos += display.Offset;
				}
			}
		}

		// Set initial position relative to parent
		HWND _parentWindowHandle = NULL;
		if (createParams.ParentWindow != Window::InvalidID)
		{
			Win32Window* parentWindow = static_cast<Win32Window*>(GetParentWindow().Get());
			_parentWindowHandle = parentWindow->_handle;

			if (!createParams.DisplayIndex.has_value())
			{
				Vector2Int relativePosition = GetRelativePosition(pos, _parentWindowHandle);

				if (pos.X != CW_USEDEFAULT)
					pos.X = relativePosition.X;

				if (pos.Y != CW_USEDEFAULT)
					pos.Y = relativePosition.Y;
			}
		}

#if UNICODE || _UNICODE
		std::wstring titleStr = StringToWideString(_title.c_str());
		const wchar_t* title = titleStr.c_str();
#else
		const char* title = createParameters.Title;
#endif

		HINSTANCE hInstance = NULL;
		if (Win32EnginePlatform* win32Platform = dynamic_cast<Win32EnginePlatform*>(Engine::Get()->GetPlatform()))
		{
			hInstance = win32Platform->GetHInstance();
		}
		else
		{
			throw std::exception("Engine platform is not a Win32EnginePlatform");
		}

		_handle = CreateWindowEx(
			windowFlagsEx,
			Win32EnginePlatform::sWindowClassName,
			title,
			windowFlags,
			pos.X,
			pos.Y,
			static_cast<int>(size.Width),
			static_cast<int>(size.Height),
			_parentWindowHandle,
			NULL, // TODO: menu
			hInstance,
			this
		);

		if (!_handle)
		{
			string err = FormatString("Failed to create Win32Window (code {})", GetLastError());
			throw std::exception(err.c_str());
		}

		CocoTrace("Created Win32Window")
	}

	Win32Window::~Win32Window()
	{
		if (_handle)
		{
			DestroyWindow(_handle);
			_handle = NULL;
		}

		CocoTrace("Destroyed Win32Window")
	}

	void Win32Window::Show()
	{
		CheckWindowHandle()

		::ShowWindow(_handle, SW_SHOW);
		UpdateFullscreenState(IsFullscreen());
	}

	void Win32Window::SetTitle(const char* title)
	{
#ifdef UNICODE
		std::wstring titleWStr = StringToWideString(title);
		const wchar_t* titleStr = titleWStr.c_str();
#else
		const char* titleStr = title;
#endif // UNICODE

		_title = string(title);
		SetWindowText(_handle, titleStr);
	}

	void Win32Window::SetIsFullscreen(bool fullscreen)
	{
		if (IsFullscreen() == fullscreen)
			return;

		UpdateFullscreenState(fullscreen);
	}

	void Win32Window::SetPosition(const Vector2Int& position, bool relativeToParent)
	{
		CheckWindowHandle()

		Vector2Int pos = position;

		if (relativeToParent)
		{
			const Win32Window* parent = static_cast<const Win32Window*>(GetParentWindow().Get());
			pos = GetRelativePosition(position, parent->_handle);
		}

		if (!::SetWindowPos(
			_handle,
			NULL,
			pos.X, pos.Y,
			0, 0,
			SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER
		))
		{
			CocoError("Failed to set window position (code {})", ::GetLastError())
		}
	}

	Vector2Int Win32Window::GetPosition(bool relativeToParent) const
	{
		CheckWindowHandle()

		RECT rect{};
		::GetWindowRect(_handle, &rect);
		Vector2Int pos(rect.left, rect.top);

		if (relativeToParent)
		{
			const Win32Window* parent = static_cast<const Win32Window*>(GetParentWindow().Get());
			pos = GetRelativePosition(pos, parent->_handle);
		}

		return pos;
	}

	void Win32Window::SetClientAreaSize(const SizeInt& size)
	{
		CheckWindowHandle()

		DWORD windowFlags = ::GetWindowLong(_handle, GWL_STYLE);
		DWORD windowFlagsEx = ::GetWindowLong(_handle, GWL_EXSTYLE);

		SizeInt adjustedSize = GetAdjustedWindowSize(size, windowFlags, windowFlagsEx);

		if (!::SetWindowPos(
			_handle,
			NULL,
			0, 0,
			static_cast<int>(size.Width), static_cast<int>(size.Height),
			SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER)
			)
		{
			CocoError("Failed to set window size (code {})", ::GetLastError())
		}
	}

	SizeInt Win32Window::GetClientAreaSize() const
	{
		CheckWindowHandle()

		RECT rect{};
		::GetClientRect(_handle, &rect);

		return SizeInt(rect.right - rect.left, rect.bottom - rect.top);
	}

	uint16 Win32Window::GetDPI() const
	{
#ifdef COCO_HIGHDPI_SUPPORT
		CheckWindowHandle()

		UINT dpi = ::GetDpiForWindow(_handle);

		return static_cast<uint16>(dpi);
#else
		return DefaultDPI;
#endif
	}

	void Win32Window::SetState(WindowState state)
	{
		CheckWindowHandle()

		UpdateState(state);
	}

	WindowState Win32Window::GetState() const
	{
		CheckWindowHandle()

		DWORD windowStyle = ::GetWindowLong(_handle, GWL_STYLE);

		if ((windowStyle & WS_MINIMIZE) == WS_MINIMIZE)
			return WindowState::Minimized;
		else if ((WINDOW_BUFFER_SIZE_EVENT & WS_MAXIMIZE) == WS_MAXIMIZE)
			return WindowState::Maximized;

		return WindowState::Default;
	}

	void Win32Window::Focus()
	{
		CheckWindowHandle()

		::SetActiveWindow(_handle);
	}

	bool Win32Window::HasFocus() const
	{
		CheckWindowHandle()

		return ::GetActiveWindow() == _handle;
	}

	bool Win32Window::IsVisible() const
	{
		SizeInt size = GetClientAreaSize();

		return IsWindowVisible(_handle) && size.Width > 0 && size.Height > 0;
	}

	SharedRef<Rendering::GraphicsPresenterSurface> Win32Window::CreateSurface()
	{
		if (!Rendering::RenderService::Get())
			throw std::exception("No RenderService is active");

		Win32EnginePlatform* platform = static_cast<Win32EnginePlatform*>(Engine::Get()->GetPlatform());

		return platform->CreateSurfaceForWindow(Rendering::RenderService::cGet()->GetPlatform()->GetName(), *this);
	}

	DWORD Win32Window::GetWindowFlags(bool canResize, bool isFullscreen)
	{
		DWORD flags = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

		if (canResize)
			flags |= WS_THICKFRAME | WS_MAXIMIZEBOX;

		if (isFullscreen)
			flags &= ~WS_OVERLAPPEDWINDOW;

		return flags;
	}

	SizeInt Win32Window::GetAdjustedWindowSize(const SizeInt& clientSize, DWORD windowFlags, DWORD exWindowFlags)
	{
		// We want the sizing to be relative to the client space within the window (not the title-bar, etc.)
		// So we need to calculate the border size and offset our position/sizing respectively
		RECT borderRect = { 0, 0, 0, 0 };
		AdjustWindowRectEx(&borderRect, windowFlags, FALSE, exWindowFlags);

		return SizeInt(clientSize.Width + (borderRect.right - borderRect.left), clientSize.Height + (borderRect.bottom - borderRect.top));
	}

	Vector2Int Win32Window::GetRelativePosition(const Vector2Int& position, HWND relativeTo)
	{
		POINT p{position.X == CW_USEDEFAULT ? 0 : position.X, position.Y == CW_USEDEFAULT ? 0 : position.Y };
		::MapWindowPoints(HWND_DESKTOP, relativeTo, &p, 1);
		return Vector2Int(p.x, p.y);
	}

	void Win32Window::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
	{
		try
		{
			switch (message)
			{
			case WM_CLOSE:
				Close();
				break;
			case WM_SIZE:
			{
				LPRECT rect = reinterpret_cast<LPRECT>(lParam);

				const WORD width = LOWORD(lParam);
				const WORD height = HIWORD(lParam);
				SizeInt size = GetClientAreaSize();

				// Update if coming out of minimized state
				if (size.Width == 0 && size.Height == 0 && width > 0 && height > 0)
					UpdateFullscreenState(IsFullscreen());

				HandleResized();
				break;
			}
#ifdef COCO_HIGHDPI_SUPPORT
			case WM_DPICHANGED:
			{
				const UINT newDpiY = HIWORD(wParam);
				const UINT newDpiX = LOWORD(wParam);

				LPRECT suggestedRect = reinterpret_cast<LPRECT>(lParam);
				SetWindowPos(_handle, NULL,
					suggestedRect->left, suggestedRect->top,
					suggestedRect->right - suggestedRect->left, suggestedRect->bottom - suggestedRect->top,
					SWP_NOZORDER | SWP_NOACTIVATE);

				try
				{
					OnDPIChanged.Invoke(GetDPI());
				}
				catch (const std::exception& ex)
				{
					CocoError("Error invoking Win32Window::OnDPIChanged: {}", ex.what())
				}
				break;
			}
#endif
			case WM_MOVE:
			{
				const int16 x = LOWORD(lParam);
				const int16 y = HIWORD(lParam);

				try
				{
					OnPositionChanged.Invoke(Vector2Int(x, y));
				}
				catch (const std::exception& ex)
				{
					CocoError("Error invoking Win32Window::OnPositionChanged: {}", ex.what())
				}
				break;
			}
			//case WM_STYLECHANGED:
			//{
			//	LPSTYLESTRUCT style = reinterpret_cast<LPSTYLESTRUCT>(lParam);
			//	break;
			//}
			case WM_SETFOCUS:
			case WM_KILLFOCUS:
			{
				try
				{
					OnFocusChanged.Invoke(message == WM_SETFOCUS);
				}
				catch (const std::exception& ex)
				{
					CocoError("Error invoking Win32Window::OnFocusChanged: {}", ex.what())
				}
				break;
			}
			default:
				break;
			}
		}
		catch (...)
		{
			Engine::Get()->CrashWithException();
		}
	}

	void Win32Window::UpdateState(WindowState state)
	{
		switch (state)
		{
		case WindowState::Minimized:
			ShowWindow(_handle, SW_MINIMIZE);
			break;
		case WindowState::Maximized:
		{
			if (!_isFullscreen && _canResize)
			{
				ShowWindow(_handle, SW_MAXIMIZE);
			}
			break;
		}
		case WindowState::Default:
		default:
		{
			if (!_isFullscreen)
				ShowWindow(_handle, SW_RESTORE);

			break;
		}
		}
	}

	void Win32Window::UpdateFullscreenState(bool fullscreen)
	{
		// https://devblogs.microsoft.com/oldnewthing/20100412-00/?p=14353

		// Restore the window frame if we're not fullscreen anymore
		if (!fullscreen && _isFullscreen)
		{
			DWORD windowStyle = ::GetWindowLong(_handle, GWL_STYLE);
			::SetWindowLong(_handle, GWL_STYLE, windowStyle | GetWindowFlags(_canResize, false));
			::SetWindowPlacement(_handle, &_restorePlacement);
			::SetWindowPos(_handle,
				NULL,
				0, 0,
				0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

			_isFullscreen = false;
		}
		else if (fullscreen)
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

				_isFullscreen = true;
			}
		}
	}
}