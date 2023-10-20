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

#ifdef COCO_SERVICE_INPUT
#include <Coco/Input/InputService.h>
#endif

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
		_focusOnShow(createParams.FocusOnShow),
		_styleFlags(createParams.StyleFlags),
		_cursorVisible(true),
		_cursorConfineMode(CursorConfineMode::None),
		_cursorConfined(false),
		_handle(nullptr),
		_restorePlacement{sizeof(WINDOWPLACEMENT)}
	{
		DWORD windowFlags = 0;
		DWORD windowFlagsEx = 0;
		GetWindowFlags(_canResize, _isFullscreen, _styleFlags, windowFlags, windowFlagsEx);

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

				if (pos.X == CW_USEDEFAULT || pos.Y == CW_USEDEFAULT)
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
		if (Win32EnginePlatform* win32Platform = dynamic_cast<Win32EnginePlatform*>(&Engine::Get()->GetPlatform()))
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

		// Create the presenter surface
		EnsurePresenterSurface();

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

		::ShowWindow(_handle, _focusOnShow ? SW_SHOW : SW_SHOWNA);
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

	void Win32Window::SetPosition(const Vector2Int& position, bool clientAreaPosition, bool relativeToParent)
	{
		CheckWindowHandle()

		Vector2Int pos = position;

		if (relativeToParent && HasParent())
		{
			const Win32Window* parent = static_cast<const Win32Window*>(GetParentWindow().Get());
			pos = GetRelativePosition(position, parent->_handle);
		}

		if (clientAreaPosition)
		{
			DWORD windowFlags = ::GetWindowLong(_handle, GWL_STYLE);
			DWORD windowFlagsEx = ::GetWindowLong(_handle, GWL_EXSTYLE);

			// We want the position to be relative to the client space within the window (not the title-bar, etc.)
			// So we need to calculate the border size and offset our position respectively
			RECT borderRect = { 0, 0, 0, 0 };
			AdjustWindowRectEx(&borderRect, windowFlags, FALSE, windowFlagsEx);

			pos.X += borderRect.left;
			pos.Y += borderRect.top;
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

	Vector2Int Win32Window::GetPosition(bool clientAreaPosition, bool relativeToParent) const
	{
		CheckWindowHandle()

		RECT rect{};
		::GetWindowRect(_handle, &rect);
		Vector2Int pos(rect.left, rect.top);

		if (relativeToParent && HasParent())
		{
			const Win32Window* parent = static_cast<const Win32Window*>(GetParentWindow().Get());
			pos = GetRelativePosition(pos, parent->_handle);
		}

		if (clientAreaPosition)
		{
			DWORD windowFlags = ::GetWindowLong(_handle, GWL_STYLE);
			DWORD windowFlagsEx = ::GetWindowLong(_handle, GWL_EXSTYLE);

			// We want the position to be relative to the client space within the window (not the title-bar, etc.)
			// So we need to calculate the border size and offset our position respectively
			RECT borderRect = { 0, 0, 0, 0 };
			AdjustWindowRectEx(&borderRect, windowFlags, FALSE, windowFlagsEx);

			pos.X -= borderRect.left;
			pos.Y -= borderRect.top;
		}

		return pos;
	}

	void Win32Window::SetClientAreaSize(const SizeInt& size)
	{
		CheckWindowHandle()

		DWORD windowFlags = ::GetWindowLong(_handle, GWL_STYLE);
		DWORD windowFlagsEx = ::GetWindowLong(_handle, GWL_EXSTYLE);

		SizeInt adjustedSize = GetAdjustedWindowSize(size, windowFlags, windowFlagsEx);

		SetSize(adjustedSize);
	}

	SizeInt Win32Window::GetClientAreaSize() const
	{
		CheckWindowHandle()

		RECT rect{};
		::GetClientRect(_handle, &rect);

		return SizeInt(rect.right - rect.left, rect.bottom - rect.top);
	}

	void Win32Window::SetSize(const SizeInt& windowSize)
	{
		if (!::SetWindowPos(
			_handle,
			NULL,
			0, 0,
			static_cast<int>(windowSize.Width), static_cast<int>(windowSize.Height),
			SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER)
			)
		{
			CocoError("Failed to set window size (code {})", ::GetLastError())
		}
	}

	SizeInt Win32Window::GetSize() const
	{
		SizeInt clientSize = GetClientAreaSize();

		DWORD windowFlags = ::GetWindowLong(_handle, GWL_STYLE);
		DWORD windowFlagsEx = ::GetWindowLong(_handle, GWL_EXSTYLE);

		return GetAdjustedWindowSize(clientSize, windowFlags, windowFlagsEx);
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
		CheckWindowHandle()

		SizeInt size = GetClientAreaSize();

		return IsWindowVisible(_handle) && size.Width > 0 && size.Height > 0;
	}

	void Win32Window::SetCursorVisibility(bool isVisible)
	{
		if (_cursorVisible == isVisible)
			return;

		CheckWindowHandle()

		::ShowCursor(isVisible);
		_cursorVisible = isVisible;
	}

	void Win32Window::SetCursorConfineMode(CursorConfineMode mode)
	{
		if (mode == _cursorConfineMode)
			return;

		_cursorConfineMode = mode;
		UpdateCursorConfineState(HasFocus());
	}

	SharedRef<Rendering::GraphicsPresenterSurface> Win32Window::CreateSurface()
	{
		CheckWindowHandle()

		if (!Rendering::RenderService::Get())
			throw std::exception("No RenderService is active");

		Win32EnginePlatform& platform = static_cast<Win32EnginePlatform&>(Engine::Get()->GetPlatform());

		return platform.CreateSurfaceForWindow(Rendering::RenderService::cGet()->GetPlatform().GetName(), *this);
	}

	void Win32Window::GetWindowFlags(bool canResize, bool isFullscreen, WindowStyleFlags styleFlags, DWORD& outStyle, DWORD& outExStyle)
	{
		outStyle = WS_OVERLAPPEDWINDOW;

		if (!canResize)
			outStyle &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);

		if ((styleFlags & WindowStyleFlags::NoDecoration) == WindowStyleFlags::NoDecoration)
			outStyle = WS_POPUP;

		if (isFullscreen)
		{
			outStyle = 0;
		}

		if ((styleFlags & WindowStyleFlags::NoTaskbarIcon) == WindowStyleFlags::NoTaskbarIcon)
		{
			outExStyle = WS_EX_TOOLWINDOW;
		}
		else
		{
			outExStyle = WS_EX_APPWINDOW;
		}

		if ((styleFlags & WindowStyleFlags::TopMost) == WindowStyleFlags::TopMost)
		{
			outExStyle |= WS_EX_TOPMOST;
		}
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

	bool Win32Window::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
	{
		try
		{
			switch (message)
			{
			case WM_CLOSE:
				Close();
				return true;
			case WM_SIZE:
			{
				// This can be called before CreateWindow returns, and we won't have a handle yet, so ignore for now
				if (!_handle)
					return false;

				LPRECT rect = reinterpret_cast<LPRECT>(lParam);

				const WORD width = LOWORD(lParam);
				const WORD height = HIWORD(lParam);
				SizeInt size = GetClientAreaSize();

				// Update if coming out of minimized state
				if (size.Width == 0 && size.Height == 0 && width > 0 && height > 0)
					UpdateFullscreenState(IsFullscreen());

				UpdateCursorConfineState(false);

				HandleResized();
				return true;
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

				UpdateCursorConfineState(false);

				try
				{
					OnDPIChanged.Invoke(GetDPI());
				}
				catch (const std::exception& ex)
				{
					CocoError("Error invoking Win32Window::OnDPIChanged: {}", ex.what())
				}
				return true;
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

				UpdateCursorConfineState(false);

				return true;
			}
			case WM_SETFOCUS:
			case WM_KILLFOCUS:
			{
				bool focused = message == WM_SETFOCUS;

				UpdateCursorConfineState(focused);

				try
				{
					OnFocusChanged.Invoke(focused);
				}
				catch (const std::exception& ex)
				{
					CocoError("Error invoking Win32Window::OnFocusChanged: {}", ex.what())
				}

				// Release the mouse if we captured it
				if (!focused && ::GetCapture() == _handle)
					::ReleaseCapture();

#ifdef COCO_SERVICE_INPUT
				if (!focused)
				{
					// If the next window is not one of our managed ones, make sure input gets reset
					HWND windowHandle = (HWND)wParam;
					LONG_PTR userPtr = GetWindowLongPtr(windowHandle, GWLP_USERDATA);
					Input::InputService* input = Input::InputService::Get();

					if (!userPtr && input)
					{
						input->LostFocus();
					}
				}
#endif
				return true;
			}
			case WM_LBUTTONDOWN:
			case WM_MBUTTONDOWN:
			case WM_RBUTTONDOWN:
			case WM_XBUTTONDOWN:
			case WM_MOUSEMOVE:
			case WM_NCMOUSEMOVE:
			case WM_INPUT:
			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
			case WM_KEYUP:
			case WM_SYSKEYUP:
			case WM_MOUSEWHEEL:
			case WM_LBUTTONUP:
			case WM_MBUTTONUP:
			case WM_RBUTTONUP:
			case WM_XBUTTONUP:
			case WM_ACTIVATEAPP:
			case WM_CHAR:
				return HandleInputMessage(message, wParam, lParam);		
			default:
				break;
			}
		}
		catch (...)
		{
			Engine::Get()->CrashWithException();
		}

		return false;
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
			DWORD windowStyleEx = 0;
			GetWindowFlags(_canResize, false, _styleFlags, windowStyle, windowStyleEx);

			::SetWindowLong(_handle, GWL_STYLE, windowStyle);
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

	void Win32Window::UpdateCursorConfineState(bool shouldCapture)
	{
		// Unconfine the cursor if needed
		if (_cursorConfineMode == CursorConfineMode::None || (!shouldCapture && _cursorConfined))
		{
			::ClipCursor(NULL);
			_cursorConfined = false;
			return;
		}

		CheckWindowHandle()

		// Get the client rectangle screen coordinates
		RECT clientRect{};
		::GetClientRect(_handle, &clientRect);
		RECT screenRect = clientRect;
		::MapWindowPoints(_handle, HWND_DESKTOP, (LPPOINT)(&screenRect), 2);

		POINT cursorPos{};
		if (!::GetCursorPos(&cursorPos))
		{
			CocoError("Failed to get cursor position - code {}", ::GetLastError())
		}

		// The cursor is not inside the client rectangle, so don't confine it
		if (cursorPos.x < screenRect.left || cursorPos.x > screenRect.right ||
			cursorPos.y < screenRect.top || cursorPos.y > screenRect.bottom)
			return;

		RECT confineRect{};

		switch (_cursorConfineMode)
		{
		case CursorConfineMode::ClientArea:
		{
			confineRect = screenRect;
			break;
		}
		case CursorConfineMode::LockedCenter:
		{
			int width = screenRect.right - screenRect.left;
			int height = screenRect.bottom - screenRect.top;
			confineRect.left = screenRect.left + width / 2;
			confineRect.top = screenRect.top + height / 2;
			confineRect.right = confineRect.left + 1;
			confineRect.bottom = confineRect.top + 1;
			break;
		}
		case CursorConfineMode::LockedInPlace:
		{
			POINT pos{};
			Assert(::GetCursorPos(&pos))

			confineRect.left = pos.x;
			confineRect.top = pos.y;
			confineRect.right = confineRect.left + 1;
			confineRect.bottom = confineRect.top + 1;

			break;
		}
		default:
			break;
		}

		if (::ClipCursor(&confineRect))
		{
			_cursorConfined = true;
		}
		else
		{
			CocoError("Failed to confine cursor - code {}", ::GetLastError())
		}
	}

	bool Win32Window::HandleInputMessage(UINT message, WPARAM wParam, LPARAM lParam)
	{
#ifdef COCO_SERVICE_INPUT
		using namespace Coco::Input;

		InputService* input = InputService::Get();

		if (!input)
			return false;

		Mouse& mouse = input->GetMouse();
		Keyboard& keyboard = input->GetKeyboard();

#endif
		switch (message)
		{
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		{
			// https://learn.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input#keystroke-message-flags

			WPARAM mappedParam = wParam;
			WORD keyFlags = HIWORD(lParam);
			WORD scancode = LOBYTE(keyFlags);
			BOOL isExtendedKey = (keyFlags & KF_EXTENDED) == KF_EXTENDED;
			bool released = (keyFlags & KF_UP) == KF_UP;

			if (isExtendedKey)
				scancode = MAKEWORD(scancode, 0xE0);

			switch (wParam)
			{
			case VK_SHIFT:
			case VK_CONTROL:
			case VK_MENU:
				mappedParam = LOWORD(::MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX));
				break;
			default:
				break;
			}

#ifdef COCO_SERVICE_INPUT
			keyboard.UpdateKeyState(static_cast<KeyboardKey>(mappedParam), !released);
#endif
			return true;
		}
		case WM_MOUSEMOVE:
		case WM_NCMOUSEMOVE:
		{
			POINT p{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

			// Mouse move is in client coordinates, so convert to screen coordinates
			if (message == WM_MOUSEMOVE)
			{
				::MapWindowPoints(_handle, HWND_DESKTOP, &p, 1);
			}

#ifdef COCO_SERVICE_INPUT
			mouse.UpdatePositionState(Vector2Int(p.x, p.y));
#endif
			return true;
		}
		case WM_INPUT:
		{
			UINT dwSize = sizeof(RAWINPUT);
			static BYTE lpb[sizeof(RAWINPUT)];

			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

			RAWINPUT* raw = (RAWINPUT*)lpb;

			if (raw->header.dwType == RIM_TYPEMOUSE)
			{
#ifdef COCO_SERVICE_INPUT
				mouse.UpdateMoveDeltaState(Vector2Int(raw->data.mouse.lLastX, raw->data.mouse.lLastY));
#endif
			}

			return true;
		}
		case WM_MOUSEWHEEL:
		{
			int yDelta = GET_WHEEL_DELTA_WPARAM(wParam);

			if (yDelta != 0)
			{
				// Flatten the z delta to be platform-independent
				yDelta = (yDelta >= 0) ? 1 : -1;

#ifdef COCO_SERVICE_INPUT
				mouse.UpdateScrollState(Vector2Int(0, yDelta));
#endif
			}

			return true;
		}
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_XBUTTONDOWN:
		{
			if (HasFocus() && !_cursorConfined && _cursorConfineMode != CursorConfineMode::None)
				UpdateCursorConfineState(true);

			MouseButton button = MouseButton::Left;

			switch (message)
			{
			case WM_LBUTTONDOWN:
				button = MouseButton::Left;
				break;
			case WM_MBUTTONDOWN:
				button = MouseButton::Middle;
				break;
			case WM_RBUTTONDOWN:
				button = MouseButton::Right;
				break;
			case WM_XBUTTONDOWN:
				button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? MouseButton::Button3 : MouseButton::Button4;
				break;
			default:
				break;
			}

			// Capture the mouse if we haven't already
			if (::GetCapture() == nullptr)
				::SetCapture(_handle);

#ifdef COCO_SERVICE_INPUT
			mouse.UpdateButtonState(button, true);
#endif
			return true;
		}
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		case WM_XBUTTONUP:
		{
			MouseButton button = MouseButton::Left;

			switch (message)
			{
			case WM_LBUTTONUP:
				button = MouseButton::Left;
				break;
			case WM_MBUTTONUP:
				button = MouseButton::Middle;
				break;
			case WM_RBUTTONUP:
				button = MouseButton::Right;
				break;
			case WM_XBUTTONUP:
				button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? MouseButton::Button3 : MouseButton::Button4;
				break;
			default:
				break;
			}

			// Release the mouse if we captured it
			if (::GetCapture() == _handle)
				::ReleaseCapture();

#ifdef COCO_SERVICE_INPUT
			mouse.UpdateButtonState(button, false);
#endif
			return true;
		}
		case WM_CHAR:
		{
			int code = -1;

			if (::IsWindowUnicode(_handle))
			{
				if (wParam > 0 && wParam < 0x10000)
				{
					code = static_cast<int>(wParam);
				}
			}
			else
			{
				wchar_t wchar = 0;
				::MultiByteToWideChar(CP_UTF8, 0, (LPCCH)&wParam, 1, &wchar, 1);
				code = static_cast<int>(wchar);
			}

#ifdef COCO_SERVICE_INPUT
			if(code != -1)
				keyboard.AddUnicodeKeyEvent(code);
#endif
			return true;
		}
		case WM_ACTIVATEAPP:
		{
			if (wParam == FALSE)
			{
#ifdef COCO_SERVICE_INPUT
				// The app is unfocusing, so clear all states
				input->LostFocus();
#endif
			}
			return false;
		}
		default:
			break;
		}

		return false;
	}
}