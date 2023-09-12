#pragma once
#include <Coco\Core\Platform\EnginePlatform.h>

#include "WindowsIncludes.h"

namespace Coco::Win32
{
    /// @brief Win32 implementation of an EnginePlatform
    class Win32EnginePlatform : public EnginePlatform
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

	private:
		/// @brief Fills in the process arguments vector with arguments from Windows
		void GetProcessArgumentsFromWindows();

		/// @brief Gets timing information for the processor
		void GetTimingInfo();
    };
}