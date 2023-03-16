#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/DateTime.h>
#include "EnginePlatformTypes.h"

namespace Coco::Platform
{
	/// <summary>
	/// Thrown when the platform fails to initialize
	/// </summary>
	using PlatformInitializeException = Exception;

	/// <summary>
	/// Thrown when a platform operation fails
	/// </summary>
	using PlatformOperationException = Exception;

	/// <summary>
	/// An interface for communicating with the underlying platform the engine is running on
	/// </summary>
	class COCOAPI IEnginePlatform
	{
	public:
		virtual ~IEnginePlatform() = default;

		/// <summary>
		/// Called when the engine has initialized and all services have started
		/// </summary>
		virtual void Start() = 0;

		/// <summary>
		/// Gets the command line arguments passed to the application
		/// </summary>
		/// <param name="arguments">A list that will be populated with the command line arguments</param>
		virtual void GetPlatformCommandLineArguments(List<string>& arguments) const noexcept = 0;

		/// <summary>
		/// Called by the engine when platform-specific messages should be handled
		/// </summary>
		virtual void HandlePlatformMessages() = 0;

		/// <summary>
		/// Called to fill out the neccessary rendering extensions for the given render hardware interface
		/// </summary>
		/// <param name="renderingRHIEnum">The type of the render hardware interface being used</param>
		/// <param name="includePresentationExtensions">If true, extensions for presenting should be included</param>
		/// <param name="extensionNames">A list that will be populated with the required render extensions</param>
		virtual void GetPlatformRenderingExtensions(int renderingRHI, bool includePresentationExtensions, List<string>& extensionNames) const noexcept = 0;

		/// <summary>
		/// Gets the current UTC time from the platform
		/// </summary>
		/// <returns>The current UTC time</returns>
		virtual DateTime GetPlatformUtcTime() const = 0;

		/// <summary>
		/// Gets the current local time from the platform
		/// </summary>
		/// <returns>The current local time</returns>
		virtual DateTime GetPlatformLocalTime() const = 0;

		/// <summary>
		/// Gets the amount of time the machine has been running
		/// </summary>
		/// <returns>The number of seconds the machine has been running</returns>
		virtual double GetPlatformTimeSeconds() const = 0;

		/// <summary>
		/// Writes a message to the platform's console output
		/// </summary>
		/// <param name="message">The message to write</param>
		/// <param name="color">The color of the message's text</param>
		/// <param name="isError">If true, the message will be written to the error stream</param>
		virtual void WriteToPlatformConsole(const string& message, ConsoleColor color, bool isError) = 0;
		
		/// <summary>
		/// Sets if the platform's console window is visible
		/// </summary>
		/// <param name="isVisible">If true, the console will be visible/opened, else it will be invisible/closed</param>
		virtual void SetPlatformConsoleVisible(bool isVisible) = 0;

		/// <summary>
		/// Causes the main program thread to sleep for the given amount of time
		/// </summary>
		/// <param name="milliseconds">The number of milliseconds to sleep</param>
		virtual void Sleep(unsigned long milliseconds) = 0;

		/// <summary>
		/// Shows a message box on the platform
		/// </summary>
		/// <param name="title">The title</param>
		/// <param name="message">The message</param>
		/// <param name="isError">If true, the message box will display for an error</param>
		virtual void ShowPlatformMessageBox(const string& title, const string& message, bool isError) = 0;

	protected:
		IEnginePlatform() = default;
	};
}