#pragma once

#include "WindowsIncludes.h"

#include <Coco/Core/Core.h>
#include <Coco/Core/Platform/EnginePlatform.h>
#include <Coco/Windowing/WindowingPlatform.h>

namespace Coco::Windowing
{
    class WindowingService;
}

namespace Coco::Platform::Windows
{
    /// <summary>
    /// Win32 platform implementation
    /// </summary>
    class COCOAPI EnginePlatformWindows : public EnginePlatform, public WindowingPlatform
    {
    private:
        static wchar_t s_windowClassName[];

        HINSTANCE _instance = NULL;
        bool _isConsoleOpen = false;
        double _secondsPerCycle;
        LARGE_INTEGER _clockFrequency;

        friend class WindowsWindow;

    public:
        EnginePlatformWindows(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow);
        virtual ~EnginePlatformWindows() override;

        virtual void GetPlatformCommandLineArguments(List<string>& arguments) const override;
        virtual void HandlePlatformMessages() override;
        virtual void GetPlatformRenderingExtensions(const string& renderingRHI, List<string>& extensionNames) const override;
        virtual void GetPlatformPresentationExtensions(const string& renderingRHI, List<string>& extensionNames) const override;
        virtual DateTime GetPlatformUtcTime() const override;
        virtual DateTime GetPlatformLocalTime() const override;
        virtual double GetPlatformTimeSeconds() const override;
        virtual void WriteToPlatformConsole(const string& message, ConsoleColor color, bool isError) override;
        virtual void SetPlatformConsoleVisible(bool isVisible) override;

        virtual ::Coco::Windowing::Window* CreatePlatformWindow(
            ::Coco::Windowing::WindowCreateParameters& createParameters, 
            ::Coco::Windowing::WindowingService* windowingService) override;

    private:
        /// <summary>
        ///  Converts a wide character array to a string
        /// </summary>
        /// <param name="wideString">A wide character array</param>
        /// <returns>A UTF-8 string representation of the input character array</returns>
        static string WideStringToString(LPWSTR wideString);

        /// <summary>
        /// Converts a string to a wide string
        /// </summary>
        /// <param name="string">A string</param>
        /// <returns>An equivalent wide string representation</returns>
        static std::wstring StringToWideString(string string);

        /// <summary>
        /// Message processing callback
        /// </summary>
        /// <param name="windowHandle">The handle to the window</param>
        /// <param name="message">The type of message</param>
        /// <param name="wParam">Message WParams</param>
        /// <param name="lParam">Message LParams</param>
        /// <returns>A value for Windows</returns>
        static LRESULT CALLBACK ProcessMessage(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

        /// <summary>
        /// Dispatches a window-specific message to the intended window
        /// </summary>
        /// <param name="windowHandle">The window handle</param>
        /// <param name="message">The type of message</param>
        /// <param name="wParam">Message WParams</param>
        /// <param name="lParam">Message LParams</param>
        static void HandleWindowMessage(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

        /// <summary>
        /// Registers the window class with Windows
        /// </summary>
        void RegisterWindowClass();

        /// <summary>
        /// Shows the console
        /// </summary>
        void ShowConsole();

        /// <summary>
        /// Hides the console
        /// </summary>
        void HideConsole();
    };
}

