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
    class COCOAPI EnginePlatformWindows final : public Singleton<EnginePlatformWindows>,
        public IEnginePlatform,
        public IRenderingPlatform,
        public IWindowingPlatform
    {
        friend class ManagedRef<EnginePlatformWindows>;

    private:
        static const wchar_t* s_windowClassName;

        HINSTANCE _instance;
        bool _isConsoleOpen = false;
        double _secondsPerCycle;
        LARGE_INTEGER _clockFrequency;

#ifdef COCO_SERVICE_WINDOWING
        friend class WindowsWindow;
#endif

    protected:
        EnginePlatformWindows(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow);

    public:
        ~EnginePlatformWindows() final;

        EnginePlatformWindows() = delete;
        EnginePlatformWindows(const EnginePlatformWindows&) = delete;
        EnginePlatformWindows& operator=(const EnginePlatformWindows&) = delete;
        EnginePlatformWindows(EnginePlatformWindows&&) = delete;
        EnginePlatformWindows& operator=(EnginePlatformWindows&&) = delete;

        /// @brief Converts a wide character array to a string
        /// @param wideString A wide character array
        /// @return A UTF-8 string representation of the input character array
        static string WideStringToString(const LPWSTR wideString);

        void Start() final;
        void GetCommandLineArguments(List<string>& arguments) const noexcept final;
        void HandlePlatformMessages() noexcept final;
        DateTime GetUtcTime() const noexcept final;
        DateTime GetLocalTime() const noexcept final;
        double GetRunningTimeSeconds() const final;
        void WriteToConsole(const string& message, ConsoleColor color, bool isError) final;
        void SetConsoleVisible(bool isVisible) noexcept final;
        void Sleep(unsigned long milliseconds) noexcept final;
        void ShowMessageBox(const string& title, const string& message, bool isError) final;

        void GetRenderingExtensions(int renderingRHI, bool includePresentationExtensions, List<string>& extensionNames) const noexcept final;

        ManagedRef<Windowing::Window> CreatePlatformWindow(const Windowing::WindowCreateParameters& createParameters) final;
        List<::Coco::Windowing::DisplayInfo> GetDisplays() const final;

    private:
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

        /// @brief Callback for enumertating monitors
        /// @param monitor The monitor
        /// @param context The device context
        /// @param rect The rect parameter
        /// @param data The user-set data parameter
        /// @return True if enumeration should continue
        static BOOL MonitorEnumCallback(HMONITOR monitor, HDC context, LPRECT rect, LPARAM data);

        /// @brief Registers the window class with Windows
        void RegisterWindowClass();

        /// @brief Shows the console
        void ShowConsole() noexcept;

        /// @brief Hides the console
        void HideConsole() noexcept;
    };
}

