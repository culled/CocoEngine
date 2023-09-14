#include "Windowpch.h"
#include "WindowService.h"
#include "WindowingPlatform.h"

#include <Coco/Core/Engine.h>

namespace Coco::Windowing
{
	WindowService::WindowService(bool dpiAware) :
		_windows{}
	{
		if (WindowingPlatform* windowPlatform = dynamic_cast<WindowingPlatform*>(Engine::Get()->GetPlatform()))
		{
#ifdef COCO_HIGHDPI_SUPPORT
			windowPlatform->SetDPIAwareMode(dpiAware);
#else
			CocoTrace("Engine was not built with high-dpi support. Skipping setting dpi awareness")
#endif
		}
		else
		{
			throw std::exception("Platform does not support windowing");
		}

		CocoTrace("WindowService initialized")
	}

	WindowService::~WindowService()
	{
		_windows.clear();

		CocoTrace("WindowService shutdown")
	}

	Window* WindowService::CreateWindow(const WindowCreateParams& createParams)
	{
		if (WindowingPlatform* windowPlatform = dynamic_cast<WindowingPlatform*>(Engine::Get()->GetPlatform()))
		{
			if (_windows.size() > 0 && !windowPlatform->SupportsMultipleWindows())
			{
				CocoError("Platform does not support multiple windows")
				return nullptr;
			}

			auto& window = _windows.emplace_back(windowPlatform->CreatePlatformWindow(createParams));

			return window.get();
		}

		CocoError("Platform does not support creating windows")
		return nullptr;
	}

	Window* WindowService::GetMainWindow()
	{
		if(_windows.size() > 0)
			return _windows.front().get();

		return nullptr;
	}

	Window* WindowService::GetWindow(const WindowID& windowID)
	{
		auto it = std::find_if(_windows.begin(), _windows.end(), [windowID](const UniqueRef<Window>& w)
			{
				return w->ID == windowID;
			});

		if (it != _windows.end())
			return it->get();

		return nullptr;
	}

	const Window* WindowService::GetWindow(const WindowID& windowID) const
	{
		auto it = std::find_if(_windows.cbegin(), _windows.cend(), [windowID](const UniqueRef<Window>& w)
			{
				return w->ID == windowID;
			});

		if (it != _windows.end())
			return it->get();

		return nullptr;
	}

	std::vector<DisplayInfo> WindowService::GetDisplays() const
	{
		if (const WindowingPlatform* windowPlatform = dynamic_cast<const WindowingPlatform*>(Engine::cGet()->GetPlatform()))
		{
			return windowPlatform->GetDisplays();
		}
		else
		{
			CocoError("Platform does not support enumerating displays")
			return std::vector<DisplayInfo>();
		}
	}

	std::vector<Window*> WindowService::GetVisibleWindows() const
	{
		std::vector<Window*> visibleWindows;

		for (const auto& window : _windows)
			if (window->IsVisible())
				visibleWindows.push_back(window.get());

		return visibleWindows;
	}

	void WindowService::WindowClosed(Window& window)
	{
		bool isMainWindow = GetMainWindow() == &window;

		// Stop the program if the main window was closed
		if (isMainWindow)
		{
			Engine::Get()->GetMainLoop()->Stop();
		}
		else
		{
			auto it = std::find_if(_windows.begin(), _windows.end(), [windowPtr = &window](const UniqueRef<Window>& p) {
				return p.get() == windowPtr;
				});

			if (it == _windows.end())
			{
				CocoError("Window was not created through the WindowService")
				return;
			}

			_windows.erase(it);
		}
	}
}