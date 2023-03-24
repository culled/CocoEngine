#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/DateTime.h>
#include "EnginePlatformTypes.h"
#include "EnginePlatformExceptions.h"

namespace Coco::Platform
{
	/// @brief An interface for communicating with the underlying platform the engine is running on
	class COCOAPI IEnginePlatform
	{
	protected:
		IEnginePlatform() = default;

	public:
		virtual ~IEnginePlatform() = default;

		/// @brief Called when the engine has initialized and all services have started
		virtual void Start() = 0;

		/// @brief Gets the command line arguments passed to the application
		/// @param arguments A list that will be populated with the command line arguments
		virtual void GetCommandLineArguments(List<string>& arguments) const noexcept = 0;

		/// @brief Called by the engine when platform-specific messages should be handled
		virtual void HandlePlatformMessages() = 0;

		/// @brief Gets the current UTC time from the platform
		/// @return The current UTC time
		virtual DateTime GetUtcTime() const = 0;

		/// @brief Gets the current local time from the platform
		/// @return The current local time 
		virtual DateTime GetLocalTime() const = 0;

		/// @brief Gets the amount of time the machine has been running
		/// @return The number of seconds the machine has been running
		virtual double GetRunningTimeSeconds() const = 0;

		/// @brief Writes a message to the platform's console output
		/// @param message The message to write
		/// @param color The color of the message's text
		/// @param isError If true, the message will be written to the error stream
		virtual void WriteToConsole(const string& message, ConsoleColor color, bool isError) = 0;
		
		/// @brief Sets the visibility of the platform's console window
		/// @param isVisible If true, the console will be visible/opened, else it will be invisible/closed
		virtual void SetConsoleVisible(bool isVisible) = 0;

		/// @brief Causes the main program thread to sleep for the given amount of time
		/// @param milliseconds The number of milliseconds to sleep
		virtual void Sleep(unsigned long milliseconds) = 0;

		/// @brief Shows a pop-up message box on the platform
		/// @param title The title
		/// @param message The message
		/// @param isError If true, the message box will display for an error
		virtual void ShowMessageBox(const string& title, const string& message, bool isError) = 0;
	};
}