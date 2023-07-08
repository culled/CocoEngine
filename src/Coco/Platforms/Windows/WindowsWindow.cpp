#include "WindowsWindow.h"

#include "EnginePlatformWindows.h"

#include <Coco/Core/Logging/Logger.h>
#include <Coco/Windowing/WindowingService.h>
#include <Coco/Rendering/Graphics/Resources/PresenterSurfaceInitializationInfo.h>
#include <Coco/Core/Services/EngineServiceManager.h>
#include <Coco/Rendering/RenderingService.h>

#define CheckWindowHandle() Assert(_handle != NULL)

namespace Coco::Platform::Windows
{
	WindowsWindow::WindowsWindow(const Coco::Windowing::WindowCreateParameters& createParameters) :
		Window()
	{
		if (Rendering::RenderingService::Get() == nullptr)
			throw Windowing::WindowCreateException("Failed to find an active RenderingService");

		_instance = EnginePlatformWindows::Get()->_instance;
		_size = createParameters.InitialSize;

#if UNICODE || _UNICODE
		std::wstring title = StringToWideString(createParameters.Title);
#else
		string title = createParameters.Title;
#endif

		int windowFlags = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		int windowFlagsEx = WS_EX_APPWINDOW;

		if (createParameters.IsResizable)
			windowFlags |= WS_MAXIMIZEBOX | WS_THICKFRAME;

		switch (createParameters.InitialState)
		{
		case Windowing::WindowState::Minimized:
			windowFlags |= WS_MINIMIZE;
			break;
		case Windowing::WindowState::Maximized:
		{
			if(createParameters.IsResizable)
				windowFlags |= WS_MAXIMIZE;
			break;
		}
		default:
			break;
		}

		int x = CW_USEDEFAULT;
		int y = CW_USEDEFAULT;

		// We want the sizing/positioning to be relative to the client space within the window (not the title-bar, etc.)
		// So we need to calculate the border size and offset our position/sizing respectively
		RECT borderRect = {0, 0, 0, 0};
		AdjustWindowRectEx(&borderRect, windowFlags, FALSE, windowFlagsEx);
		
		if (createParameters.InitialPosition.has_value())
		{
			x = createParameters.InitialPosition->X + borderRect.left;
			y = createParameters.InitialPosition->Y + borderRect.top;
		}

		const int width = createParameters.InitialSize.Width + (borderRect.right - borderRect.left);
		const int height = createParameters.InitialSize.Height + (borderRect.bottom - borderRect.top);

		_handle = CreateWindowEx(
			windowFlagsEx,
			EnginePlatformWindows::s_windowClassName,
			title.c_str(),
			windowFlags,
			x,
			y,
			width,
			height,
			NULL, // TODO: parented windows
			NULL, // Menu
			_instance,
			this);

		if (_handle == NULL)
			throw Windowing::WindowCreateException(FormattedString("Failed to create window: {}", GetLastError()));

		_presenter = Rendering::RenderingService::Get()->GetPlatform()->CreatePresenter(createParameters.Title);

		LogTrace(Windowing::WindowingService::Get()->GetLogger(), "Created Windows window");
	}

	WindowsWindow::~WindowsWindow()
	{
		if (_handle != NULL)
		{
			DestroyWindow(_handle);
			_handle = NULL;
		}

		LogTrace(Windowing::WindowingService::Get()->GetLogger(), "Destroyed Windows window");
	}

	string WindowsWindow::GetTitle() const noexcept
	{
		CheckWindowHandle();

		int titleLength = GetWindowTextLength(_handle) + 1;
		LPTSTR rawTitle = nullptr;
		string title;

		try
		{
			rawTitle = new TCHAR[titleLength];

			if (rawTitle != NULL)
			{
				int count = GetWindowText(_handle, rawTitle, titleLength);

#ifdef UNICODE
				title = EnginePlatformWindows::WideStringToString(rawTitle);
#else
				title = string((LPSTR)rawTitle, count);
#endif
			}
		}
		catch (const Exception& e)
		{
			LogError(Windowing::WindowingService::Get()->GetLogger(), FormattedString("Failed to get window title: {}", e.what()));
		}

		if (rawTitle != NULL)
			delete[] rawTitle;

		return title;
	}

	void WindowsWindow::Show() noexcept
	{
		CheckWindowHandle();

		ShowWindow(_handle, SW_SHOW);
	}

	void WindowsWindow::Minimize() noexcept
	{
		CheckWindowHandle();

		ShowWindow(_handle, SW_MINIMIZE);
	}

	bool WindowsWindow::GetIsVisible() const noexcept
	{
		return IsWindowVisible(_handle);
	}

	void WindowsWindow::SetupPresenterSurface()
	{
		_presenter->InitializeSurface(Rendering::PresenterWin32SurfaceInitializationInfo(_handle, _instance));
	}

	void WindowsWindow::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_CLOSE:
			Close();
			break;
		case WM_SIZE:
		{
			const UINT width = LOWORD(lParam);
			const UINT height = HIWORD(lParam);
			_size = SizeInt(static_cast<int>(width), static_cast<int>(height));
			HandleResized();
			break;
		}
		default:
			//LogTrace(WindowingService->GetLogger(), FormattedString("Got message {0}", message));
			break;
		}
	}
}