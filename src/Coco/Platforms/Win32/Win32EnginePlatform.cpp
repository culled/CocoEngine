#include "CPWinpch.h"
#include "Win32EnginePlatform.h"

#include <Coco/Core/Engine.h>

#ifdef COCO_SERVICES_WINDOWING
#include "Win32Window.h"
#endif // COCO_SERVICES_WINDOWING

#ifdef COCO_SERVICES_INPUT
#include <Coco/Input/InputService.h>
#endif

namespace Coco::Platforms::Win32
{
	Win32EnginePlatform::Win32EnginePlatform(HINSTANCE hInstance) :
		_hInstance(hInstance),
		_consoleOpen(false)
	{
		GetProcessArgumentsFromWindows();
		GetTimingInfo();

#ifdef COCO_SERVICES_RENDERING
		_renderingExtensions = CreateSharedRef<Win32RenderingExtensions>();
#endif

#ifdef COCO_SERVICES_WINDOWING
		if (!RegisterWindowClass())
		{
			throw std::exception("Failed to register window class");
		}

		_windowExtensions = CreateSharedRef<Win32WindowExtensions>();
#endif
	}

	Win32EnginePlatform::~Win32EnginePlatform()
	{
		_processArguments.clear();
	}

	double Win32EnginePlatform::GetSeconds() const
	{
		LARGE_INTEGER count{};

		if (QueryPerformanceCounter(&count) && _clockFrequency > 0)
		{
			return static_cast<double>(count.QuadPart) * _secondsPerCycle;
		}

		// Fallback to non-high resolution clock
		return GetLocalTime().GetTotalSeconds();
	}

	DateTime Win32EnginePlatform::GetLocalTime() const
	{
		SYSTEMTIME time{};

		::GetLocalTime(&time);

		return DateTime(time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
	}

	DateTime Win32EnginePlatform::GetUtcTime() const
	{
		SYSTEMTIME time{};

		::GetSystemTime(&time);

		return DateTime(time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
	}

	void Win32EnginePlatform::Sleep(uint64 milliseconds)
	{
		::Sleep(static_cast<DWORD>(milliseconds));
	}

	void Win32EnginePlatform::ProcessMessages()
	{
		MSG message = {};
		while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE) > 0)
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}

	void Win32EnginePlatform::PushProcessArgument(const char* arg)
	{
		_processArguments.emplace_back(arg);
	}

	bool Win32EnginePlatform::HasProcessArgument(const char* arg) const
	{
		auto it = std::find_if(_processArguments.cbegin(), _processArguments.cend(), [arg](const string& s)
			{
				return strcmp(arg, s.c_str()) == 0;
			});

		return it != _processArguments.cend();
	}

	string Win32::Win32EnginePlatform::GetProcessArgument(const char* arg) const
	{
		auto it = std::find_if(_processArguments.cbegin(), _processArguments.cend(), [arg](const string& s)
			{
				return strcmp(arg, s.c_str()) == 0;
			});

		if (it == _processArguments.cend())
			return string();

		const string& wholeArg = *it;
		size_t deliminator = wholeArg.find_first_of('=');

		if (deliminator == string::npos)
			return wholeArg;

		return wholeArg.substr(deliminator + 1);
	}

	void Win32EnginePlatform::ShowConsoleWindow(bool show)
	{
		if (show == _consoleOpen)
			return;

		if (show)
		{
			if (AllocConsole())
			{
				_consoleOpen = true;
			}
			else
			{
				//CocoError("Failed to open console window")
			}
		}
		else
		{
			FreeConsole();
			_consoleOpen = false;
		}
	}

	void Win32EnginePlatform::WriteToConsole(const char* message, ConsoleColor color)
	{
		HANDLE outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);

		WORD textAttributes = 0;

		switch (color)
		{
		case ConsoleColor::Red:
			textAttributes = FOREGROUND_RED;
			break;
		case ConsoleColor::Green:
			textAttributes = FOREGROUND_GREEN;
			break;
		case ConsoleColor::Blue:
			textAttributes = FOREGROUND_BLUE;
			break;
		case ConsoleColor::Yellow:
			textAttributes = FOREGROUND_RED | FOREGROUND_GREEN;
			break;
		case ConsoleColor::Magenta:
			textAttributes = FOREGROUND_RED | FOREGROUND_BLUE;
			break;
		case ConsoleColor::Cyan:
			textAttributes = FOREGROUND_GREEN | FOREGROUND_BLUE;
			break;
		case ConsoleColor::White:
			textAttributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
			break;
		default:
			break;
		}

		SetConsoleTextAttribute(outputHandle, textAttributes);

#if UNICODE || _UNICODE
		std::wstring wStr = StringToWideString(message);
		const wchar_t* str = wStr.c_str();
		DWORD length = static_cast<DWORD>(wStr.length());
#else
		const char* str = message;
		DWORD length = strlen(message);
#endif // UNICODE || _UNICODE

		LPDWORD charactersWritten = 0;
		WriteConsole(outputHandle, str, length, charactersWritten, 0);

		OutputDebugString(str);
	}

	void Win32EnginePlatform::ShowMessageBox(const char* title, const char* message, bool isError)
	{
#if UNICODE || _UNICODE
		std::wstring titleWStr = StringToWideString(title);
		wstring messageWStr = StringToWideString(message);

		const wchar_t* titleStr = titleWStr.c_str();
		const wchar_t* messageStr = messageWStr.c_str();
#else
		const char* titleStr = title;
		const char* messageStr = message;
#endif

		UINT flags = MB_OK;

		if (isError)
			flags |= MB_ICONERROR;
		else
			flags |= MB_ICONINFORMATION;

		MessageBox(NULL, messageStr, titleStr, flags);
	}

	LRESULT Win32EnginePlatform::ProcessMessage(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
#ifdef COCO_SERVICES_WINDOWING
		case WM_NCCREATE:
		case WM_CREATE:
		{
			// Set our window pointer so we can access it when the window receives messages
			const LPCREATESTRUCT createPtr = reinterpret_cast<LPCREATESTRUCT>(lParam);
			const Win32Window* windowPtr = static_cast<Win32Window*>(createPtr->lpCreateParams);
			SetWindowLongPtr(windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(windowPtr));
			break;
		}
		case WM_CLOSE: // A window has been requested to close
			DispatchWindowMessage(windowHandle, message, wParam, lParam);
			return 0;
		case WM_SIZE: // A window has been resized
		case WM_MOVE: // A window has finished moving
		case WM_DPICHANGED: // A window's dpi changed
		case WM_SETFOCUS: // A window got focus
		case WM_KILLFOCUS: // A window lost focus
			DispatchWindowMessage(windowHandle, message, wParam, lParam);
			break;
#endif
		case WM_ERASEBKGND:
			// Erasing will be handled by us to prevent flicker
			return 1;
#ifdef COCO_SERVICES_INPUT
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
		case WM_MOUSEMOVE:
		case WM_MOUSEWHEEL:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONDBLCLK:
		case WM_MBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
		case WM_RBUTTONUP:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONDBLCLK:
		case WM_XBUTTONUP:
			HandleInputMessage(message, wParam, lParam);
			break;
#endif
		default: break;
		}
		return DefWindowProc(windowHandle, message, wParam, lParam);
	}

	void Win32EnginePlatform::GetProcessArgumentsFromWindows()
	{
		int numArgs;
		LPWSTR* rawArguments = ::CommandLineToArgvW(::GetCommandLineW(), &numArgs);

		if (!rawArguments)
		{
			return;
		}

		try
		{
			std::span<LPWSTR> args(rawArguments, numArgs);

			for (auto it : args)
			{
				_processArguments.push_back(WideStringToString(it));
			}
		}
		catch (...)
		{
		}

		LocalFree(rawArguments);
	}

	void Win32EnginePlatform::GetTimingInfo()
	{
		LARGE_INTEGER freq;
		if (QueryPerformanceFrequency(&freq))
		{
			_clockFrequency = freq.QuadPart;
			_secondsPerCycle = 1.0 / static_cast<double>(_clockFrequency);
		}
		else
		{
			_clockFrequency = 0;
			_secondsPerCycle = 0.0;
		}

		_startTime = GetSeconds();
	}

#ifdef COCO_SERVICES_RENDERING
	void Win32EnginePlatform::SetRenderingExtensions(SharedRef<Win32RenderingExtensions> renderingExtensions)
	{
		_renderingExtensions = renderingExtensions;
	}

	void Win32EnginePlatform::GetPlatformRenderingExtensions(const char* renderRHIName, bool includePresentationExtensions, std::vector<const char*>& extensions) const
	{
		_renderingExtensions->GetRenderingExtensions(renderRHIName, includePresentationExtensions, extensions);
	}
#endif

#ifdef COCO_SERVICES_WINDOWING
	const wchar_t* Win32EnginePlatform::sWindowClassName = L"CocoWindow";

	void Win32::Win32EnginePlatform::SetWindowExtensions(SharedRef<Win32WindowExtensions> windowExtensions)
	{
		_windowExtensions = windowExtensions;
	}

	SharedRef<Rendering::GraphicsPresenterSurface> Win32::Win32EnginePlatform::CreateSurfaceForWindow(const char* renderRHIName, const Win32Window& window) const
	{
		return _windowExtensions->CreateSurfaceForWindow(renderRHIName, window);
	}

	ManagedRef<Windowing::Window> Win32EnginePlatform::CreatePlatformWindow(const Windowing::WindowCreateParams& createParams)
	{
		return CreateManagedRef<Win32Window>(createParams);
	}

	BOOL MonitorEnumCallback(HMONITOR monitor, HDC context, LPRECT rect, LPARAM data)
	{
		std::vector<Windowing::DisplayInfo>* displays = reinterpret_cast<std::vector<Windowing::DisplayInfo>*>(data);

		MONITORINFOEX monitorInfo{ sizeof(MONITORINFOEX) };
		::GetMonitorInfo(monitor, &monitorInfo);

		string name = WideStringToString(monitorInfo.szDevice);

		Windowing::DisplayInfo info{};
		info.Name = WideStringToString(monitorInfo.szDevice);
		info.Offset = Vector2Int(monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top);
		info.Resolution = SizeInt(monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top);
		info.IsPrimary = (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) == MONITORINFOF_PRIMARY;

#ifdef COCO_HIGHDPI_SUPPORT
		UINT dpiX, dpiY;
		::GetDpiForMonitor(monitor, MDT_DEFAULT, &dpiX, &dpiY);

		info.DPI = static_cast<uint16>(dpiX);
#else
		info.DPI = Windowing::Window::DefaultDPI;
#endif

		displays->push_back(info);

		return TRUE;
	}

	std::vector<Windowing::DisplayInfo> Win32EnginePlatform::GetDisplays() const
	{
		std::vector<Windowing::DisplayInfo> displayInfos;
		EnumDisplayMonitors(NULL, NULL, &MonitorEnumCallback, reinterpret_cast<LPARAM>(&displayInfos));

		return displayInfos;
	}

	void Win32EnginePlatform::SetDPIAwareMode(bool dpiAware)
	{
#ifdef COCO_HIGHDPI_SUPPORT
		// NOTE: if compiling for versions of windows earlier than Win10 Creators Update, this may need to change
		DPI_AWARENESS_CONTEXT mode = dpiAware ? DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 : DPI_AWARENESS_CONTEXT_UNAWARE;

		if (!::SetProcessDpiAwarenessContext(mode))
		{
			CocoError("Failed to set the DPI aware mode (code {})", GetLastError())
		}
#else
		CocoError("Cannot set dpi awareness: Engine was not built with high-dpi support")
#endif
	}

	bool Win32EnginePlatform::RegisterWindowClass()
	{
		HICON icon = LoadIcon(_hInstance, IDI_APPLICATION);
		WNDCLASS windowClass = { };
		windowClass.lpfnWndProc = &Win32EnginePlatform::ProcessMessage;
		windowClass.hInstance = _hInstance;
		windowClass.lpszClassName = sWindowClassName;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hIcon = icon;
		windowClass.hCursor = LoadCursor(NULL, IDC_ARROW); // TODO: change cursor type
		windowClass.hbrBackground = NULL;
		windowClass.style = CS_DBLCLKS; // Get doubleclicks

		return RegisterClass(&windowClass);
	}

	void Win32EnginePlatform::DispatchWindowMessage(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
	{
		LONG_PTR userPtr = GetWindowLongPtr(windowHandle, GWLP_USERDATA);
		Win32Window* window = reinterpret_cast<Win32Window*>(userPtr);

		if (!window)
		{
			CocoError("Target window of message was null")
			return;
		}

		window->ProcessMessage(message, wParam, lParam);
	}
#endif

#ifdef COCO_SERVICES_INPUT
	void Win32EnginePlatform::HandleInputMessage(UINT message, WPARAM wParam, LPARAM lParam)
	{
		ServiceManager* services = ServiceManager::Get();

		using namespace Coco::Input;
		if (!services->HasService<InputService>())
			return;

		InputService* input = services->GetService<InputService>();

		switch (message)
		{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			input->GetKeyboard()->UpdateKeyState(static_cast<KeyboardKey>(wParam), true);
			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			input->GetKeyboard()->UpdateKeyState(static_cast<KeyboardKey>(wParam), false);
			break;
		case WM_MOUSEMOVE:
		{
			const int x = GET_X_LPARAM(lParam);
			const int y = GET_Y_LPARAM(lParam);

			input->GetMouse()->UpdatePositionState(Vector2Int(x, y));
			break;
		}
		case WM_MOUSEWHEEL:
		{
			int yDelta = GET_WHEEL_DELTA_WPARAM(wParam);

			if (yDelta != 0)
			{
				// Flatten the z delta to be platform-independent
				yDelta = (yDelta >= 0) ? 1 : -1;

				input->GetMouse()->UpdateScrollState(Vector2Int(0, yDelta));
			}

			break;
		}
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONUP:
		{
			if (message == WM_LBUTTONDBLCLK)
			{
				//input->GetMouse()->DoubleClicked(MouseButton::Left);
			}

			input->GetMouse()->UpdateButtonState(MouseButton::Left, message != WM_LBUTTONUP);
			break;
		}
		case WM_MBUTTONDOWN:
		case WM_MBUTTONDBLCLK:
		case WM_MBUTTONUP:
		{
			if (message == WM_MBUTTONDBLCLK)
			{
				//input->GetMouse()->DoubleClicked(MouseButton::Middle);
			}

			input->GetMouse()->UpdateButtonState(MouseButton::Middle, message != WM_MBUTTONUP);
			break;
		}
		case WM_RBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
		case WM_RBUTTONUP:
		{
			if (message == WM_RBUTTONDBLCLK)
			{
				//input->GetMouse()->DoubleClicked(MouseButton::Right);
			}

			input->GetMouse()->UpdateButtonState(MouseButton::Right, message != WM_RBUTTONUP);
			break;
		}
		case WM_XBUTTONDOWN:
		case WM_XBUTTONDBLCLK:
		case WM_XBUTTONUP:
		{
			const Input::MouseButton button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? MouseButton::Button3 : MouseButton::Button4;

			if (message == WM_XBUTTONDBLCLK)
			{
				//input->GetMouse()->DoubleClicked(button);
			}

			input->GetMouse()->UpdateButtonState(button, message != WM_XBUTTONUP);
			break;
		}
		default:
			break;
		}
	}
#endif
}