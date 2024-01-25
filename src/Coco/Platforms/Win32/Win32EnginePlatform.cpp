#include "CPWinpch.h"
#include "Win32EnginePlatform.h"

#include <Coco/Core/Engine.h>

#ifdef COCO_SERVICE_WINDOWING
#include "Win32Window.h"
#endif // COCO_SERVICE_WINDOWING

#ifdef COCO_SERVICE_INPUT
#include <hidusage.h>
#endif

namespace Coco::Platforms::Win32
{
	Win32PlatformOperationException::Win32PlatformOperationException(const string& message) :
		Exception(message)
	{}

	Win32EnginePlatform::Win32EnginePlatform(HINSTANCE hInstance) :
		_hInstance(hInstance),
		_consoleOpen(false)
	{
		GetProcessArgumentsFromWindows();
		GetTimingInfo();

#ifdef COCO_SERVICE_RENDERING
		_renderingExtensions = CreateSharedRef<Win32RenderingExtensions>();
#endif

#ifdef COCO_SERVICE_WINDOWING
		if (!RegisterWindowClass())
		{
			throw Win32PlatformOperationException("Failed to register window class");
		}

		_windowExtensions = CreateSharedRef<Win32WindowExtensions>();
#endif

#ifdef COCO_SERVICE_INPUT
		SetupRawInput();
#endif
	}

	Win32EnginePlatform::~Win32EnginePlatform()
	{
		_processArguments.clear();
	}

	string Win32EnginePlatform::GetWin32ErrorMessage(DWORD error)
	{
#if UNICODE || _UNICODE
		std::wstring str(512, L'\0');
#else
		string str(512, '\0');
#endif // UNICODE || _UNICODE

		// Try to get the message from the system errors.
		DWORD chars = ::FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			error,
			0,
			str.data(),
			static_cast<DWORD>(str.size()),
			NULL
		);

		if (chars > 0)
		{
#if UNICODE || _UNICODE
			return WideStringToString(str.c_str());
#else
			return str;
#endif
		}

		return FormatString("Code {}", error);
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
				size_t endPartPos = s.find('=');

				if (endPartPos != string::npos)
					return s.substr(0, endPartPos) == arg;

				return s == arg;
			});

		return it != _processArguments.cend();
	}

	string Win32::Win32EnginePlatform::GetProcessArgument(const char* arg) const
	{
		auto it = std::find_if(_processArguments.cbegin(), _processArguments.cend(), [arg](const string& s)
			{
				size_t endPartPos = s.find('=');

				if (endPartPos != string::npos)
					return s.substr(0, endPartPos) == arg;

				return s == arg;
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
				CocoError("Failed to open console window")
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

	string Win32EnginePlatform::ShowOpenFileDialog(const std::vector<std::pair<const char*, const char*>>& filters)
	{
#ifdef COCO_SERVICE_WINDOWING
		return _windowExtensions->ShowOpenFileDialog(filters);
#else
		CocoError("The windowing service needs to be included to use file dialogs")
		return string();
#endif
	}

	string Win32EnginePlatform::ShowSaveFileDialog(const std::vector<std::pair<const char*, const char*>>& filters)
	{
#ifdef COCO_SERVICE_WINDOWING
		return _windowExtensions->ShowSaveFileDialog(filters);
#else
		CocoError("The windowing service needs to be included to use file dialogs")
			return string();
#endif
	}

	LRESULT Win32EnginePlatform::ProcessMessage(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
#ifdef COCO_SERVICE_WINDOWING
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
		case WM_ACTIVATEAPP: // The app got/lost focus
		case WM_KEYDOWN: // A key was pressed
		case WM_SYSKEYDOWN: // A key was pressed
		case WM_KEYUP: // A key was released
		case WM_SYSKEYUP: // A key was released
		case WM_MOUSEMOVE: // The mouse moved in the client area
		case WM_NCMOUSEMOVE: // The mouse moved in the non-client area
		case WM_INPUT: // Raw input message
		case WM_MOUSEWHEEL: // The mouse wheel moved
		case WM_LBUTTONDOWN: // The left mouse button was pressed
		case WM_MBUTTONDOWN: // The middle mouse button was pressed
		case WM_RBUTTONDOWN: // The right mouse button was pressed
		case WM_XBUTTONDOWN: // A mouse button was pressed
		case WM_LBUTTONUP: // The left mouse button was released
		case WM_MBUTTONUP: // The middle mouse button was released
		case WM_RBUTTONUP: // The right mouse button was released
		case WM_XBUTTONUP: // A mouse button was released
		case WM_CHAR: // A unicode character was entered
		case WM_KILLFOCUS: // A window lost focus
		{
			bool handled = DispatchWindowMessage(windowHandle, message, wParam, lParam);
			if (handled)
				return 0;
			break;
		}
#endif
		case WM_ERASEBKGND:
			// Erasing will be handled by us to prevent flicker
			return 1;
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
			std::transform(args.begin(), args.end(),
				std::back_inserter(_processArguments),
				[](LPWSTR str) 
				{ 
					return WideStringToString(str); 
				}
			);
		}
		catch (...)
		{
			CocoError("Failed to get process arguments from Windows")
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

#ifdef COCO_SERVICE_RENDERING
	void Win32EnginePlatform::SetRenderingExtensions(SharedRef<Win32RenderingExtensions> renderingExtensions)
	{
		_renderingExtensions = renderingExtensions;
	}

	void Win32EnginePlatform::GetPlatformRenderingExtensions(const string& renderRHIName, bool includePresentationExtensions, std::vector<string>& extensions) const
	{
		CocoAssert(_renderingExtensions, "Rendering extensions were null. Did you forget to set them via SetRenderingExtensions()?")
		_renderingExtensions->GetRenderingExtensions(renderRHIName, includePresentationExtensions, extensions);
	}
#endif

#ifdef COCO_SERVICE_WINDOWING
	const wchar_t* Win32EnginePlatform::sWindowClassName = L"CocoWindow";

	void Win32EnginePlatform::SetWindowExtensions(SharedRef<Win32WindowExtensions> windowExtensions)
	{
		_windowExtensions = windowExtensions;
	}

	UniqueRef<Rendering::PresenterSurface> Win32EnginePlatform::CreateSurfaceForWindow(const string& renderRHIName, const Win32Window& window) const
	{
		CocoAssert(_windowExtensions, "Window extensions were null. Did you forget to set them via SetWindowExtensions()?")
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
			CocoError("Failed to set the DPI aware mode: {}", GetWin32ErrorMessage(::GetLastError()))
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
		//windowClass.style = CS_DBLCLKS; // Get doubleclicks

		return RegisterClass(&windowClass);
	}

	bool Win32EnginePlatform::DispatchWindowMessage(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
	{
		LONG_PTR userPtr = GetWindowLongPtr(windowHandle, GWLP_USERDATA);

		if (!userPtr)
		{
			CocoError("Target window of message was null")
			return false;
		}

		Win32Window* window = reinterpret_cast<Win32Window*>(userPtr);

		return window->ProcessMessage(message, wParam, lParam);
	}
#endif

#ifdef COCO_SERVICE_INPUT
	void Win32::Win32EnginePlatform::SetupRawInput()
	{
		std::array<RAWINPUTDEVICE, 1> rids = {};
		RAWINPUTDEVICE& rid = rids.at(0);
		rid.usUsagePage = HID_USAGE_PAGE_GENERIC;
		rid.usUsage = HID_USAGE_GENERIC_MOUSE;
		rid.dwFlags = 0;
		rid.hwndTarget = NULL;

		if (!RegisterRawInputDevices(rids.data(), static_cast<UINT>(rids.size()), sizeof(rid)))
		{
			CocoError("Failed to register raw input: {}", GetWin32ErrorMessage(::GetLastError()));
		}
	}
#endif
}