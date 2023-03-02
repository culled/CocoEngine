#include "EnginePlatformWindows.h"

#include "WindowsIncludes.h"
#include "WindowsWindow.h"

#include <Coco/Core/Engine.h>
#include <Coco/Core/Logging/Logger.h>
#include <Coco/Core/Services/EngineServiceManager.h>
#include <Coco/Windowing/WindowingService.h>
#include <Coco/Core/Input/InputService.h>
#include <Coco/Core/Input/Keyboard.h>
#include <Coco/Core/Input/Mouse.h>
#include <Coco/Rendering/Graphics/GraphicsPlatformTypes.h>

namespace Coco::Platform::Windows
{
	wchar_t EnginePlatformWindows::s_windowClassName[] = L"CocoWindowClass";
	Input::InputService* EnginePlatformWindows::_inputService = nullptr;

	EnginePlatformWindows::EnginePlatformWindows(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) : IEnginePlatform(),
		_instance(hInstance),
		_isConsoleOpen(false)
	{
		if (!QueryPerformanceFrequency(&_clockFrequency))
		{
			string err = FormattedString("Unable to query performance frequency: {}", GetLastError());
			throw Exception(err.c_str());
		}

		_secondsPerCycle = 1.0 / static_cast<double>(_clockFrequency.QuadPart);

		RegisterWindowClass();
	}

	EnginePlatformWindows::~EnginePlatformWindows()
	{
		HideConsole();
		_instance = NULL;
	}

	void EnginePlatformWindows::Start()
	{
		if (!Engine::Get()->GetServiceManager()->TryFindService<Input::InputService>(&_inputService))
		{
			LogWarning(Engine::Get()->GetLogger(), "Could not find an input service. Input will not be handled");
			_inputService = nullptr;
		}
	}

	void EnginePlatformWindows::GetPlatformCommandLineArguments(List<string>& arguments) const
	{
		int numArgs;
		LPWSTR* rawArguments = ::CommandLineToArgvW(::GetCommandLineW(), &numArgs);

		if (rawArguments == NULL)
		{
			LogError(Engine::Get()->GetLogger(), "Could not retrieve command line arguments");
			return;
		}

		try
		{
			for (int i = 0; i < numArgs; i++)
			{
				// We need to convert the wide-string input to our UTF-8 internal representation
				arguments.Add(WideStringToString(rawArguments[i]));
			}
		}
		catch (Exception& err)
		{
			LogError(Engine::Get()->GetLogger(), FormattedString("Could not narrow command line arguments: {}", err.what()));
		}

		LocalFree(rawArguments);
	}

	void EnginePlatformWindows::HandlePlatformMessages()
	{
		MSG message = {};
		while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE) > 0)
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}

	void EnginePlatformWindows::GetPlatformRenderingExtensions(int renderingRHI, bool includePresentationExtensions, List<string>& extensionNames) const
	{
		switch(static_cast<Rendering::RenderingRHI>(renderingRHI))
		{
		case Rendering::RenderingRHI::Vulkan:
			extensionNames.Add("VK_KHR_win32_surface");
			break;
		default:
			break;
		}
	}

	DateTime EnginePlatformWindows::GetPlatformUtcTime() const
	{
		SYSTEMTIME systemTime = {};
		GetSystemTime(&systemTime);

		return DateTime(systemTime.wYear, systemTime.wMonth, systemTime.wDay, systemTime.wHour, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds);
	}

	DateTime EnginePlatformWindows::GetPlatformLocalTime() const
	{
		SYSTEMTIME localTime = {};
		GetLocalTime(&localTime);

		return DateTime(localTime.wYear, localTime.wMonth, localTime.wDay, localTime.wHour, localTime.wMinute, localTime.wSecond, localTime.wMilliseconds);
	}

	double EnginePlatformWindows::GetPlatformTimeSeconds() const
	{
		LARGE_INTEGER cycles = {};
		if(!QueryPerformanceCounter(&cycles))
		{
			string err = FormattedString("Unable to query performance counter: {}", GetLastError());
			throw Exception(err.c_str());
		}

		return static_cast<double>(cycles.QuadPart) * _secondsPerCycle;
	}

	void EnginePlatformWindows::WriteToPlatformConsole(const string& message, ConsoleColor color, bool isError)
	{
		if (!_isConsoleOpen)
			ShowConsole();

		HANDLE outputHandle = GetStdHandle(isError ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE);

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
		std::wstring str = StringToWideString(message);
#else
		std::string str = message;
#endif

		LPDWORD charactersWritten = 0;
		WriteConsole(outputHandle, str.c_str(), static_cast<DWORD>(str.length()), charactersWritten, 0);

		OutputDebugString(str.c_str());
	}

	void EnginePlatformWindows::SetPlatformConsoleVisible(bool isVisible)
	{
		if (isVisible == _isConsoleOpen)
			return;

		if (isVisible)
		{
			ShowConsole();
		}
		else
		{
			HideConsole();
		}
	}

	Managed<::Coco::Windowing::Window> EnginePlatformWindows::CreatePlatformWindow(
		::Coco::Windowing::WindowCreateParameters& createParameters, 
		::Coco::Windowing::WindowingService* windowingService)
	{
		return CreateManaged<WindowsWindow>(createParameters, windowingService, this);
	}

	void EnginePlatformWindows::Sleep(unsigned long milliseconds)
	{
		::Sleep(milliseconds);
	}

	void EnginePlatformWindows::ShowPlatformMessageBox(const string& title, const string& message, bool isError)
	{
#if UNICODE || _UNICODE
		std::wstring titleStr = StringToWideString(title);
		std::wstring messageStr = StringToWideString(message);
#else
		std::string titleStr = title;
		std::string messageStr = message;
#endif

		UINT flags = MB_OK;

		if (isError)
			flags |= MB_ICONERROR;
		else
			flags |= MB_ICONINFORMATION;

		MessageBox(NULL, messageStr.c_str(), titleStr.c_str(), flags);
	}

	string EnginePlatformWindows::WideStringToString(LPWSTR wideString)
	{
		int wStrLen = static_cast<int>(wcslen(wideString));
		int strLen = WideCharToMultiByte(CP_UTF8, 0, wideString, wStrLen, NULL, 0, NULL, NULL);

		string str;
		str.resize(strLen);

		WideCharToMultiByte(CP_UTF8, 0, wideString, wStrLen, &str[0], strLen, NULL, NULL);

		return str;
	}

	std::wstring EnginePlatformWindows::StringToWideString(string string)
	{
		int strLen = static_cast<int>(string.length());
		int wStrLen = MultiByteToWideChar(CP_UTF8, 0, string.c_str(), strLen, NULL, 0);

		std::wstring wStr;
		wStr.resize(wStrLen);

		MultiByteToWideChar(CP_UTF8, 0, string.c_str(), strLen, &wStr[0], wStrLen);
		return wStr;
	}

	LRESULT CALLBACK EnginePlatformWindows::ProcessMessage(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_NCCREATE:
		case WM_CREATE:
		{
			// Set our window pointer so we can access it when the window receives messages
			LPCREATESTRUCT createPtr = reinterpret_cast<LPCREATESTRUCT>(lParam);
			WindowsWindow* windowPtr = reinterpret_cast<WindowsWindow*>(createPtr->lpCreateParams);
			SetWindowLongPtr(windowHandle, GWLP_USERDATA, (LONG_PTR)windowPtr);
			break;
		}
		case WM_ERASEBKGND:
			// Erasing will be handled by us to prevent flicker
			return 1;
		case WM_CLOSE: // A window has been requested to close
			HandleWindowMessage(windowHandle, message, wParam, lParam);
			return 0;
		case WM_SIZING: // A window is being resized
		case WM_SIZE: // A window has been resized
		case WM_MOVING: // A window is moving
		case WM_MOVE: // A window has finished moving
			HandleWindowMessage(windowHandle, message, wParam, lParam);
			break;
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
			HandleInputMessage(windowHandle, message, wParam, lParam);
			break;
		}

		return DefWindowProc(windowHandle, message, wParam, lParam);
	}

	void EnginePlatformWindows::HandleWindowMessage(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
	{
		LONG_PTR userPtr = GetWindowLongPtr(windowHandle, GWLP_USERDATA);
		WindowsWindow* windowsWindow = reinterpret_cast<WindowsWindow*>(userPtr);

		if (windowsWindow == nullptr)
		{
			LogWarning(Coco::Engine::Get()->GetLogger(), "Window handle was null");
			return;
		}

		windowsWindow->ProcessMessage(message, wParam, lParam);
	}

	void EnginePlatformWindows::HandleInputMessage(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (_inputService == nullptr)
			return;

		switch (message)
		{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			_inputService->GetKeyboard()->UpdateKeyState(static_cast<Input::KeyboardKey>(wParam), true);
			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			_inputService->GetKeyboard()->UpdateKeyState(static_cast<Input::KeyboardKey>(wParam), false);
			break;
		case WM_MOUSEMOVE:
		{
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);

			_inputService->GetMouse()->UpdatePositionState(Vector2Int(x, y));
			break;
		}
		case WM_MOUSEWHEEL:
		{
			int yDelta = GET_WHEEL_DELTA_WPARAM(wParam);

			if (yDelta != 0)
			{
				// Flatten the z delta to be platform-independent
				yDelta = (yDelta >= 0) ? 1 : -1;

				_inputService->GetMouse()->UpdateScrollState(Vector2Int(0, yDelta));
			}

			break;
		}
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONUP:
		{
			if (message == WM_LBUTTONDBLCLK)
				_inputService->GetMouse()->DoubleClicked(Input::MouseButton::Left);

			_inputService->GetMouse()->UpdateButtonState(Input::MouseButton::Left, message != WM_LBUTTONUP);
			break;
		}
		case WM_MBUTTONDOWN:
		case WM_MBUTTONDBLCLK:
		case WM_MBUTTONUP:
		{
			if (message == WM_MBUTTONDBLCLK)
				_inputService->GetMouse()->DoubleClicked(Input::MouseButton::Middle);

			_inputService->GetMouse()->UpdateButtonState(Input::MouseButton::Middle, message != WM_MBUTTONUP);
			break;
		}
		case WM_RBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
		case WM_RBUTTONUP:
		{
			if (message == WM_RBUTTONDBLCLK)
				_inputService->GetMouse()->DoubleClicked(Input::MouseButton::Right);

			_inputService->GetMouse()->UpdateButtonState(Input::MouseButton::Right, message != WM_RBUTTONUP);
			break;
		}
		case WM_XBUTTONDOWN:
		case WM_XBUTTONDBLCLK:
		case WM_XBUTTONUP:
		{
			Input::MouseButton button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? Input::MouseButton::Button3 : Input::MouseButton::Button4;

			if (message == WM_XBUTTONDBLCLK)
				_inputService->GetMouse()->DoubleClicked(button);

			_inputService->GetMouse()->UpdateButtonState(button, message != WM_XBUTTONUP);
			break;
		}
		default:
			break;
		}
	}

	void EnginePlatformWindows::RegisterWindowClass()
	{
		HICON icon = LoadIcon(_instance, IDI_APPLICATION);
		WNDCLASS windowClass = { };
		windowClass.lpfnWndProc = &EnginePlatformWindows::ProcessMessage;
		windowClass.hInstance = _instance;
		windowClass.lpszClassName = s_windowClassName;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hIcon = icon;
		windowClass.hCursor = LoadCursor(NULL, IDC_ARROW); // TODO: change cursor type
		windowClass.hbrBackground = NULL;
		windowClass.style = CS_DBLCLKS; // Get doubleclicks

		if (!RegisterClass(&windowClass))
		{
			throw PlatformInitializeException("Could not register window class");
		}
	}

	void EnginePlatformWindows::ShowConsole()
	{
		if (_isConsoleOpen)
			return;

		if (AllocConsole())
		{
			_isConsoleOpen = true;
		}
	}

	void EnginePlatformWindows::HideConsole()
	{
		if (!_isConsoleOpen)
			return;

		FreeConsole();
		_isConsoleOpen = false;
	}
}