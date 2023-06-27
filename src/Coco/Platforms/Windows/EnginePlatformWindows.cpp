#include "EnginePlatformWindows.h"

#include "WindowsIncludes.h"

#include <Coco/Core/Engine.h>
#include <Coco/Core/Logging/Logger.h>
#include <Coco/Core/Services/EngineServiceManager.h>

#ifdef COCO_SERVICE_WINDOWING
#include "WindowsWindow.h"
#endif // COCO_SERVICE_WINDOWING

#ifdef COCO_SERVICE_INPUT
#include <Coco/Input/InputService.h>
#include <Coco/Input/Keyboard.h>
#include <Coco/Input/Mouse.h>
#endif // COCO_SERVICE_INPUT

#ifdef COCO_SERVICE_RENDERING
#include <Coco/Rendering/Graphics/GraphicsPlatformTypes.h>
#endif // COCO_SERVICE_RENDERING

namespace Coco::Platform::Windows
{
	const wchar_t* EnginePlatformWindows::s_windowClassName = L"CocoWindowClass";

	EnginePlatformWindows::EnginePlatformWindows(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) :
		_instance(hInstance)
	{
		if (!QueryPerformanceFrequency(&_clockFrequency))
			throw EnginePlatformInitializeException(FormattedString("Unable to query performance frequency: {}", GetLastError()));

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
#ifdef COCO_SERVICE_INPUT
		if (!Engine::Get()->GetServiceManager()->TryFindService<Input::InputService>(s_inputService))
		{
			LogWarning(Engine::Get()->GetLogger(), "Could not find an input service. Input will not be handled");
			s_inputService = nullptr;
		}
#endif // COCO_SERVICE_INPUT
	}

	void EnginePlatformWindows::GetCommandLineArguments(List<string>& arguments) const noexcept
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
		catch (const Exception& err)
		{
			LogError(Engine::Get()->GetLogger(), FormattedString("Could not narrow command line arguments: {}", err.what()));
		}

		LocalFree(rawArguments);
	}

	void EnginePlatformWindows::HandlePlatformMessages() noexcept
	{
		MSG message = {};
		while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE) > 0)
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}

	DateTime EnginePlatformWindows::GetUtcTime() const
	{
		SYSTEMTIME systemTime = {};
		GetSystemTime(&systemTime);

		return DateTime(systemTime.wYear, systemTime.wMonth, systemTime.wDay, systemTime.wHour, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds);
	}

	DateTime EnginePlatformWindows::GetLocalTime() const
	{
		SYSTEMTIME localTime = {};
		::GetLocalTime(&localTime);

		return DateTime(localTime.wYear, localTime.wMonth, localTime.wDay, localTime.wHour, localTime.wMinute, localTime.wSecond, localTime.wMilliseconds);
	}

	double EnginePlatformWindows::GetRunningTimeSeconds() const
	{
		LARGE_INTEGER cycles = {};
		if (!QueryPerformanceCounter(&cycles))
			throw EnginePlatformOperationException(FormattedString("Unable to query performance counter: {}", GetLastError()));

		return static_cast<double>(cycles.QuadPart) * _secondsPerCycle;
	}

	void EnginePlatformWindows::WriteToConsole(const string& message, ConsoleColor color, bool isError)
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
#endif // UNICODE || _UNICODE

		LPDWORD charactersWritten = 0;
		WriteConsole(outputHandle, str.c_str(), static_cast<DWORD>(str.length()), charactersWritten, 0);

		OutputDebugString(str.c_str());
	}

	void EnginePlatformWindows::SetConsoleVisible(bool isVisible) noexcept
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

	void EnginePlatformWindows::GetRenderingExtensions(int renderingRHI, bool includePresentationExtensions, List<string>& extensionNames) const noexcept
	{
#ifdef COCO_SERVICE_RENDERING
		try
		{
			switch (static_cast<Rendering::RenderingRHI>(renderingRHI))
			{
#ifdef COCO_RENDERING_VULKAN
			case Rendering::RenderingRHI::Vulkan:
				extensionNames.Add("VK_KHR_win32_surface");
				break;
#endif // COCO_RENDERING_VULKAN
			default:
				break;
			}
		}
		catch (const Exception& ex)
		{
			LogError(Engine::Get()->GetLogger(), FormattedString("Could not retreive platform rendering extensions: {}", ex.what()));
		}
#endif // COCO_SERVICE_RENDERING
	}

	ManagedRef<Windowing::Window> EnginePlatformWindows::CreatePlatformWindow(
		Windowing::WindowCreateParameters& createParameters, 
		Windowing::WindowingService* windowingService)
	{
#ifdef COCO_SERVICE_WINDOWING
		return CreateManagedRef<WindowsWindow>(createParameters, windowingService, this);
#else
		throw InvalidOperationException("Windowing support was not included");
#endif // COCO_SERVICE_WINDOWING
	}

	void EnginePlatformWindows::Sleep(unsigned long milliseconds) noexcept
	{
		::Sleep(milliseconds);
	}

	void EnginePlatformWindows::ShowMessageBox(const string& title, const string& message, bool isError)
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

	string EnginePlatformWindows::WideStringToString(const LPWSTR wideString)
	{
		const int wStrLen = static_cast<const int>(wcslen(wideString));
		const int strLen = WideCharToMultiByte(CP_UTF8, 0, wideString, wStrLen, NULL, 0, NULL, NULL);

		string str;
		str.resize(strLen);

		WideCharToMultiByte(CP_UTF8, 0, wideString, wStrLen, str.data(), strLen, NULL, NULL);

		return str;
	}

	std::wstring EnginePlatformWindows::StringToWideString(const string& string)
	{
		const int strLen = static_cast<const int>(string.length());
		const int wStrLen = MultiByteToWideChar(CP_UTF8, 0, string.c_str(), strLen, NULL, 0);

		std::wstring wStr;
		wStr.resize(wStrLen);

		MultiByteToWideChar(CP_UTF8, 0, string.c_str(), strLen, wStr.data(), wStrLen);
		return wStr;
	}

	LRESULT CALLBACK EnginePlatformWindows::ProcessMessage(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
#ifdef COCO_SERVICE_WINDOWING
		case WM_NCCREATE:
		case WM_CREATE:
		{
			// Set our window pointer so we can access it when the window receives messages
			const LPCREATESTRUCT createPtr = reinterpret_cast<LPCREATESTRUCT>(lParam);
			const WindowsWindow* windowPtr = static_cast<WindowsWindow*>(createPtr->lpCreateParams);
			SetWindowLongPtr(windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(windowPtr));
			break;
		}
		case WM_CLOSE: // A window has been requested to close
			HandleWindowMessage(windowHandle, message, wParam, lParam);
			return 0;
		case WM_SIZING: // A window is being resized
		case WM_SIZE: // A window has been resized
		case WM_MOVING: // A window is moving
		case WM_MOVE: // A window has finished moving
			HandleWindowMessage(windowHandle, message, wParam, lParam);
			break;
#endif
		case WM_ERASEBKGND:
			// Erasing will be handled by us to prevent flicker
			return 1;
#ifdef COCO_SERVICE_INPUT
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
#endif
		default:
			break;
		}

		return DefWindowProc(windowHandle, message, wParam, lParam);
	}

	void EnginePlatformWindows::HandleWindowMessage(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
	{
#ifdef COCO_SERVICE_WINDOWING
		LONG_PTR userPtr = GetWindowLongPtr(windowHandle, GWLP_USERDATA);
		WindowsWindow* windowsWindow = reinterpret_cast<WindowsWindow*>(userPtr);
		
		if (windowsWindow == nullptr)
		{
			LogWarning(Coco::Engine::Get()->GetLogger(), "Window handle was null");
			return;
		}

		windowsWindow->ProcessMessage(message, wParam, lParam);
#endif // COCO_SERVICE_WINDOWING
	}

#ifdef COCO_SERVICE_INPUT
	Input::InputService* EnginePlatformWindows::s_inputService = nullptr;
#endif // COCO_SERVICE_INPUT

	void EnginePlatformWindows::HandleInputMessage(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
	{
#ifdef COCO_SERVICE_INPUT
		if (s_inputService == nullptr)
			return;

		switch (message)
		{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			s_inputService->GetKeyboard()->UpdateKeyState(static_cast<Input::KeyboardKey>(wParam), true);
			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			s_inputService->GetKeyboard()->UpdateKeyState(static_cast<Input::KeyboardKey>(wParam), false);
			break;
		case WM_MOUSEMOVE:
		{
			const int x = GET_X_LPARAM(lParam);
			const int y = GET_Y_LPARAM(lParam);

			s_inputService->GetMouse()->UpdatePositionState(Vector2Int(x, y));
			break;
		}
		case WM_MOUSEWHEEL:
		{
			int yDelta = GET_WHEEL_DELTA_WPARAM(wParam);

			if (yDelta != 0)
			{
				// Flatten the z delta to be platform-independent
				yDelta = (yDelta >= 0) ? 1 : -1;

				s_inputService->GetMouse()->UpdateScrollState(Vector2Int(0, yDelta));
			}

			break;
		}
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONUP:
		{
			if (message == WM_LBUTTONDBLCLK)
				s_inputService->GetMouse()->DoubleClicked(Input::MouseButton::Left);

			s_inputService->GetMouse()->UpdateButtonState(Input::MouseButton::Left, message != WM_LBUTTONUP);
			break;
		}
		case WM_MBUTTONDOWN:
		case WM_MBUTTONDBLCLK:
		case WM_MBUTTONUP:
		{
			if (message == WM_MBUTTONDBLCLK)
				s_inputService->GetMouse()->DoubleClicked(Input::MouseButton::Middle);

			s_inputService->GetMouse()->UpdateButtonState(Input::MouseButton::Middle, message != WM_MBUTTONUP);
			break;
		}
		case WM_RBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
		case WM_RBUTTONUP:
		{
			if (message == WM_RBUTTONDBLCLK)
				s_inputService->GetMouse()->DoubleClicked(Input::MouseButton::Right);

			s_inputService->GetMouse()->UpdateButtonState(Input::MouseButton::Right, message != WM_RBUTTONUP);
			break;
		}
		case WM_XBUTTONDOWN:
		case WM_XBUTTONDBLCLK:
		case WM_XBUTTONUP:
		{
			const Input::MouseButton button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? Input::MouseButton::Button3 : Input::MouseButton::Button4;

			if (message == WM_XBUTTONDBLCLK)
				s_inputService->GetMouse()->DoubleClicked(button);

			s_inputService->GetMouse()->UpdateButtonState(button, message != WM_XBUTTONUP);
			break;
		}
		default:
			break;
		}
#endif // COCO_SERVICE_INPUT
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
			throw EnginePlatformInitializeException("Could not register window class");
	}

	void EnginePlatformWindows::ShowConsole() noexcept
	{
		if (_isConsoleOpen)
			return;

		if (AllocConsole())
		{
			_isConsoleOpen = true;
		}
	}

	void EnginePlatformWindows::HideConsole() noexcept
	{
		if (!_isConsoleOpen)
			return;

		FreeConsole();
		_isConsoleOpen = false;
	}
}