#pragma once

#include "WindowsIncludes.h"

#include <Coco/Core/Core.h>
#include <Coco/Core/Platform/IEnginePlatform.h>
#include <Coco/Windowing/IWindowingPlatform.h>

namespace Coco::Input
{
    class InputService;
}

namespace Coco::Platform::Windows
{
    /// <summary>
    /// Win32 platform implementation
    /// </summary>
    class COCOAPI EnginePlatformWindows final : public IEnginePlatform, public IWindowingPlatform
    {
    private:
        static const wchar_t* s_windowClassName;
        static Input::InputService* _inputService;

        HINSTANCE _instance;
        bool _isConsoleOpen = false;
        double _secondsPerCycle;
        LARGE_INTEGER _clockFrequency;

        friend class WindowsWindow;


    public:
        EnginePlatformWindows(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow);
        ~EnginePlatformWindows() final;

        EnginePlatformWindows() = delete;
        EnginePlatformWindows(const EnginePlatformWindows&) = delete;
        EnginePlatformWindows(EnginePlatformWindows&&) = delete;

        EnginePlatformWindows& operator=(const EnginePlatformWindows&) = delete;
        EnginePlatformWindows& operator=(EnginePlatformWindows&&) = delete;

        void Start() final;
        void GetPlatformCommandLineArguments(List<string>& arguments) const noexcept final;
        void HandlePlatformMessages() noexcept final;
        void GetPlatformRenderingExtensions(int renderingRHI, bool includePresentationExtensions, List<string>& extensionNames) const noexcept final;
        DateTime GetPlatformUtcTime() const noexcept final;
        DateTime GetPlatformLocalTime() const noexcept final;
        double GetPlatformTimeSeconds() const noexcept final;
        void WriteToPlatformConsole(const string& message, ConsoleColor color, bool isError) final;
        void SetPlatformConsoleVisible(bool isVisible) noexcept final;
        void Sleep(unsigned long milliseconds) noexcept final;
        void ShowPlatformMessageBox(const string& title, const string& message, bool isError) final;

        Managed<::Coco::Windowing::Window> CreatePlatformWindow(
            ::Coco::Windowing::WindowCreateParameters& createParameters, 
            ::Coco::Windowing::WindowingService* windowingService) final;

    private:
        /// <summary>
        ///  Converts a wide character array to a string
        /// </summary>
        /// <param name="wideString">A wide character array</param>
        /// <returns>A UTF-8 string representation of the input character array</returns>
        static string WideStringToString(const LPWSTR wideString);

        /// <summary>
        /// Converts a string to a wide string
        /// </summary>
        /// <param name="string">A string</param>
        /// <returns>An equivalent wide string representation</returns>
        static std::wstring StringToWideString(const string& string);

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
        /// Handles an input-specific message
        /// </summary>
        /// <param name="windowHandle">The window handle</param>
        /// <param name="message">The type of message</param>
        /// <param name="wParam">Message WParams</param>
        /// <param name="lParam">Message LParams</param>
        static void HandleInputMessage(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

        /// <summary>
        /// Registers the window class with Windows
        /// </summary>
        void RegisterWindowClass();

        /// <summary>
        /// Shows the console
        /// </summary>
        void ShowConsole() noexcept;

        /// <summary>
        /// Hides the console
        /// </summary>
        void HideConsole() noexcept;
    };
}

