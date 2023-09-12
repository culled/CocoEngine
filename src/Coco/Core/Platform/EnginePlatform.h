#pragma once

#include "../Types/String.h"
#include "../Types/DateTime.h"

namespace Coco
{
	/// @brief A platform that the engine runs on
	class EnginePlatform
	{
		friend class Engine;

	public:
		/// @brief Types of console colors
		enum class ConsoleColor
		{
			White,
			Red,
			Green,
			Blue,
			Yellow,
			Magenta,
			Cyan,
			Black
		};

	public:
		virtual ~EnginePlatform() = default;

		/// @brief Gets the high-resolution time (if supported) from the platform in seconds
		/// @return The high-resolution time (if supported)
		virtual double GetSeconds() const = 0;

		/// @brief Gets the high-resolution time (if supported) since the platform was initialized
		/// @return The time since the platform was initialized
		virtual double GetRunningTime() const = 0;

		/// @brief Gets the time in the local time zone of the system
		/// @return The local time
		virtual DateTime GetLocalTime() const = 0;

		/// @brief Gets the UTC time of the system
		/// @return The UTC time
		virtual DateTime GetUtcTime() const = 0;

		/// @brief Causes the program to sleep for at least the given amount of time.
		/// NOTE: the actual time spent asleep is determined by the system's scheduler and most likely will not equal the requested amount of time
		/// @param milliseconds The number of milliseconds that the program will sleep in milliseconds
		virtual void Sleep(uint64 milliseconds) = 0;

		/// @brief Processes any pending platform messages
		virtual void ProcessMessages() = 0;
		
		/// @brief Pushes an argument to the list of process arguments
		/// @param arg The argument to push
		virtual void PushProcessArgument(const char* arg) = 0;

		/// @brief Gets the process arguments
		/// @return The process arguments
		virtual const std::vector<string>& GetProcessArguments() const = 0;

		/// @brief Checks if the given argument exists in the process arguments
		/// @param arg The argument
		/// @return True if the given argument exists in the process arguments
		virtual bool HasProcessArgument(const char* arg) const = 0;

		/// @brief Shows or hides the console window (if the platform supports it)
		/// @param show If true, the console window will be shown
		virtual void ShowConsoleWindow(bool show) = 0;

		/// @brief Writes a message to the console
		/// @param message The message
		/// @param color The color for the message
		virtual void WriteToConsole(const char* message, ConsoleColor color = ConsoleColor::White) = 0;

		/// @brief Shows a message box
		/// @param title The title for the message box
		/// @param message The message
		/// @param isError If true, the message will be shown as an error
		virtual void ShowMessageBox(const char* title, const char* message, bool isError) = 0;
	};
}