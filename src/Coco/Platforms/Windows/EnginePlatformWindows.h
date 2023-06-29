#pragma once

#include <Coco/Core/Platform/IEnginePlatform.h>

#include "WindowsIncludes.h"

#include <Coco/Rendering/IRenderingPlatform.h>
#include <Coco/Windowing/IWindowingPlatform.h>

namespace Coco::Input
{
    class InputService;
}

namespace Coco::Platform::Windows
{
    /// @brief Win32 platform implementation
    class COCOAPI EnginePlatformWindows final : public IEnginePlatform, public IRenderingPlatform, public IWindowingPlatform
    {
    private:
        static const wchar_t* s_windowClassName;

#if COCO_SERVICE_INPUT
        static Input::InputService* s_inputService;
#endif

        HINSTANCE _instance;
        bool _isConsoleOpen = false;
        double _secondsPerCycle;
        LARGE_INTEGER _clockFrequency;

#ifdef COCO_SERVICE_WINDOWING
        friend class WindowsWindow;
#endif

    public:
        EnginePlatformWindows(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow);
        ~EnginePlatformWindows() final;

        EnginePlatformWindows() = delete;
        EnginePlatformWindows(const EnginePlatformWindows&) = delete;
        EnginePlatformWindows& operator=(const EnginePlatformWindows&) = delete;
        EnginePlatformWindows(EnginePlatformWindows&&) = delete;
        EnginePlatformWindows& operator=(EnginePlatformWindows&&) = delete;

        void Start() final;
        void GetCommandLineArguments(List<string>& arguments) const noexcept final;
        void HandlePlatformMessages() noexcept final;
        DateTime GetUtcTime() const final;
        DateTime GetLocalTime() const final;
        double GetRunningTimeSeconds() const final;
        void WriteToConsole(const string& message, ConsoleColor color, bool isError) final;
        void SetConsoleVisible(bool isVisible) noexcept final;
        void Sleep(unsigned long milliseconds) noexcept final;
        void ShowMessageBox(const string& title, const string& message, bool isError) final;

        void GetRenderingExtensions(int renderingRHI, bool includePresentationExtensions, List<string>& extensionNames) const noexcept final;

        ManagedRef<Windowing::Window> CreatePlatformWindow(
            const Windowing::WindowCreateParameters& createParameters, 
            Windowing::WindowingService* windowingService) final;

    private:
        /// @brief Converts a wide character array to a string
        /// @param wideString A wide character array
        /// @return A UTF-8 string representation of the input character array
        static string WideStringToString(const LPWSTR wideString);

        /// @brief Converts a string to a wide string
        /// @param string A string
        /// @return An equivalent wide string representation
        static std::wstring StringToWideString(const string& string);

        /// @brief Message processing callback
        /// @param windowHandle The handle to the window
        /// @param message The type of message
        /// @param wParam Message WParams
        /// @param lParam Message LParams
        /// @return A value for Windows
        static LRESULT CALLBACK ProcessMessage(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

        /// @brief Dispatches a window-specific message to the intended window
        /// @param windowHandle The handle to the window
        /// @param message The type of message
        /// @param wParam Message WParams
        /// @param lParam Message LParams
        static void HandleWindowMessage(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

        /// @brief Handles an input-specific message
        /// @param windowHandle The handle to the window
        /// @param message The type of message
        /// @param wParam Message WParams
        /// @param lParam Message LParams
        static void HandleInputMessage(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

        /// @brief Registers the window class with Windows
        void RegisterWindowClass();

        /// @brief Shows the console
        void ShowConsole() noexcept;

        /// @brief Hides the console
        void HideConsole() noexcept;
    };
}

