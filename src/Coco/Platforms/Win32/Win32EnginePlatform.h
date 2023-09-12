#pragma once
#include <Coco\Core\Platform\EnginePlatform.h>

#include "WindowsIncludes.h"

#ifdef COCO_SERVICES_WINDOWING
#include <Coco/Windowing/WindowingPlatform.h>
#endif

namespace Coco::Platforms::Win32
{
    /// @brief Win32 implementation of an EnginePlatform
    class Win32EnginePlatform : public EnginePlatform
#ifdef COCO_SERVICES_WINDOWING
		, public Windowing::WindowingPlatform
#endif
    {
	private:
		HINSTANCE _hInstance;
		std::vector<string> _processArguments;
		uint64 _clockFrequency;
		double _secondsPerCycle;
		double _startTime;
		bool _consoleOpen;

	public:
		Win32EnginePlatform(HINSTANCE hInstance);
		~Win32EnginePlatform();

		double GetSeconds() const final;
		double GetRunningTime() const final { return GetSeconds() - _startTime; }
		DateTime GetLocalTime() const final;
		DateTime GetUtcTime() const final;
		void Sleep(uint64 milliseconds) final;
		void ProcessMessages() final;
		void PushProcessArgument(const char* arg) final;
		const std::vector<string>& GetProcessArguments() const final { return _processArguments; }
		bool HasProcessArgument(const char* arg) const final;
		void ShowConsoleWindow(bool show) final;
		void WriteToConsole(const char* message, ConsoleColor color) final;
		void ShowMessageBox(const char* title, const char* message, bool isError) final;

		/// @brief Gets the HINSTANCE
		/// @return The HINSTANCE
		HINSTANCE GetHInstance() { return _hInstance; }

	private:
		/// @brief Callback to process window messages from Windows
		/// @param windowHandle The window the message is for
		/// @param message The message
		/// @param wParam The wParam
		/// @param lParam The lParam
		/// @return The result
		static LRESULT ProcessMessage(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

		/// @brief Fills in the process arguments vector with arguments from Windows
		void GetProcessArgumentsFromWindows();

		/// @brief Gets timing information for the processor
		void GetTimingInfo();

#ifdef COCO_SERVICES_WINDOWING
	public:
		static const wchar_t* sWindowClassName;

		bool SupportsMultipleWindows() const final { return true; }
		UniqueRef<Windowing::Window> CreatePlatformWindow(const Windowing::WindowCreateParams& createParams) final;
		std::vector<Windowing::DisplayInfo> GetDisplays() const final;
		void SetDPIAwareMode(bool dpiAware) final;

	private:
		/// @brief Dispatches a window message to the window it is intended for
		/// @param windowHandle The handler of the window the message is for
		/// @param message The message
		/// @param wParam The wParam
		/// @param lParam The lParam
		static void DispatchWindowMessage(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

		/// @brief Registers the window class with Windows
		/// @return True if the class was registered
		bool RegisterWindowClass();
#endif
    };
}