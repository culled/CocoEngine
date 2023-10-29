#include "Windowpch.h"
#include "WindowService.h"
#include "WindowingPlatform.h"

#include <Coco/Core/Engine.h>

namespace Coco::Windowing
{
	WindowService::WindowService(bool dpiAware) :
		_windows{}
	{
		if (WindowingPlatform* windowPlatform = dynamic_cast<WindowingPlatform*>(&Engine::Get()->GetPlatform()))
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

	Ref<Window> WindowService::CreateWindow(const WindowCreateParams& createParams)
	{
		if (WindowingPlatform* windowPlatform = dynamic_cast<WindowingPlatform*>(&Engine::Get()->GetPlatform()))
		{
			if (_windows.size() > 0 && !windowPlatform->SupportsMultipleWindows())
			{
				CocoError("Platform does not support multiple windows")
				return Ref<Window>();
			}

			auto& window = _windows.emplace_back(windowPlatform->CreatePlatformWindow(createParams));

			return window;
		}

		CocoError("Platform does not support creating windows")
		return Ref<Window>();
	}

	Ref<Window> WindowService::GetMainWindow() const
	{
		if(_windows.size() > 0)
			return _windows.front();

		return Ref<Window>();
	}

	Ref<Window> WindowService::GetWindow(const WindowID& windowID) const
	{
		auto it = std::find_if(_windows.begin(), _windows.end(), [windowID](const ManagedRef<Window>& w)
			{
				return w->ID == windowID;
			});

		if (it != _windows.end())
			return *it;

		return Ref<Window>();
	}

	bool WindowService::GetWindowUnderPoint(const Vector2Int& point, Ref<Window>& outWindow) const
	{
		std::vector<Ref<Window>> visibleWindows = GetVisibleWindows();

		for (Ref<Window>& window : visibleWindows)
		{
			RectInt windowRect = window->GetRect(false);

			// TODO: only get top-most window
			if (windowRect.Intersects(point))
			{
				outWindow = window;
				return true;
			}
		}

		return false;
	}

	std::vector<DisplayInfo> WindowService::GetDisplays() const
	{
		if (const WindowingPlatform* windowPlatform = dynamic_cast<const WindowingPlatform*>(&Engine::cGet()->GetPlatform()))
		{
			return windowPlatform->GetDisplays();
		}
		else
		{
			CocoError("Platform does not support enumerating displays")
			return std::vector<DisplayInfo>();
		}
	}

	std::vector<Ref<Window>> WindowService::GetVisibleWindows() const
	{
		std::vector<Ref<Window>> visibleWindows;
		std::copy_if(_windows.begin(), _windows.end(), 
			std::back_inserter(visibleWindows), 
			[](const ManagedRef<Window>& window) { return window->IsVisible(); }
		);

		return visibleWindows;
	}

	void WindowService::WindowClosed(Window& window)
	{
		bool isMainWindow = GetMainWindow().Get() == &window;

		// Stop the program if the main window was closed
		if (isMainWindow)
		{
			MainLoop::Get()->Stop();
		}
		else
		{
			auto it = std::find_if(_windows.begin(), _windows.end(), [windowPtr = &window](const ManagedRef<Window>& p) {
				return p.Get() == windowPtr;
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