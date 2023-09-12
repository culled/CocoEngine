#include "CPWinpch.h"
#include "Win32EnginePlatform.h"

namespace Coco::Win32
{
	Win32EnginePlatform::Win32EnginePlatform(HINSTANCE hInstance) :
		_hInstance(hInstance),
		_consoleOpen(false)
	{
		GetProcessArgumentsFromWindows();
		GetTimingInfo();
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
		// TODO
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
		DWORD length = wStr.length();
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
}