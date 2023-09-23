#pragma once
#include <Coco/Core/Platform/EnginePlatform.h>
#include <Coco/Core/Types/Refs.h>

#ifdef COCO_SERVICES_RENDERING
#include <Coco/Rendering/RenderingPlatform.h>
#include "Win32RenderingExtensions.h"
#endif

#ifdef COCO_SERVICES_WINDOWING
#include <Coco/Windowing/WindowingPlatform.h>
#include "Win32WindowExtensions.h"
#endif

#include "WindowsIncludes.h"

namespace Coco::Platforms::Win32
{
    /// @brief Win32 implementation of an EnginePlatform
    class Win32EnginePlatform : public EnginePlatform
#ifdef COCO_SERVICES_RENDERING
		, public Rendering::RenderingPlatform
#endif
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
		std::span<const string> GetProcessArguments() const final { return _processArguments; }
		bool HasProcessArgument(const char* arg) const final;
		string GetProcessArgument(const char* arg) const final;
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

#ifdef COCO_SERVICES_RENDERING
	public:
		SharedRef<Win32RenderingExtensions> _renderingExtensions;

		/// @brief Sets the rendering extension provider that this platform should use
		/// @param renderingExtensions The extension provider
		void SetRenderingExtensions(SharedRef<Win32RenderingExtensions> renderingExtensions);

		void GetPlatformRenderingExtensions(const char* renderRHIName, bool includePresentationExtensions, std::vector<const char*>& outExtensions) const final;
#endif

#ifdef COCO_SERVICES_WINDOWING
	public:
		static const wchar_t* sWindowClassName;
		SharedRef<Win32WindowExtensions> _windowExtensions;

		/// @brief Sets the window extension provider that this platform should use
		/// @param windowExtensions The extension provider
		void SetWindowExtensions(SharedRef<Win32WindowExtensions> windowExtensions);

		/// @brief Creates a surface for a window
		/// @param renderRHIName The name of the render RHI being used
		/// @param window The window to create the surface for
		/// @return A surface for the window
		SharedRef<Rendering::GraphicsPresenterSurface> CreateSurfaceForWindow(const char* renderRHIName, const Win32Window& window) const;

		bool SupportsMultipleWindows() const final { return true; }
		ManagedRef<Windowing::Window> CreatePlatformWindow(const Windowing::WindowCreateParams& createParams) final;
		std::vector<Windowing::DisplayInfo> GetDisplays() const final;
		void SetDPIAwareMode(bool dpiAware) final;

	private:
		/// @brief Dispatches a window message to the window it is intended for
		/// @param windowHandle The handler of the window the message is for
		/// @param message The message
		/// @param wParam The wParam
		/// @param lParam The lParam
		/// @return True if the message was handled
		static bool DispatchWindowMessage(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

		/// @brief Registers the window class with Windows
		/// @return True if the class was registered
		bool RegisterWindowClass();
#endif

#ifdef COCO_SERVICES_INPUT
	private:
		/// @brief Handles an input message
		/// @param message The message
		/// @param wParam The wParam
		/// @param lParam The lParam
		/// @return True if the message was handled
		static bool HandleInputMessage(UINT message, WPARAM wParam, LPARAM lParam);
#endif
    };
}