#include "Windowpch.h"
#include "WindowService.h"
#include "WindowingPlatform.h"

#include <Coco/Core/Engine.h>

namespace Coco::Windowing
{
	WindowService::WindowService(bool dpiAware) :
		_windows{}
	{
		WindowingPlatform* windowingPlatform = dynamic_cast<WindowingPlatform*>(&Engine::Get()->GetPlatform());
		CocoAssert(windowingPlatform, "Platform does not support windowing")

		
#ifdef COCO_HIGHDPI_SUPPORT
		windowingPlatform->SetDPIAwareMode(dpiAware);
#else
		CocoTrace("Engine was not built with high-dpi support. Skipping setting dpi awareness")
#endif

		CocoTrace("WindowService initialized")
	}

	WindowService::~WindowService()
	{
		_windows.clear();

		CocoTrace("WindowService shutdown")
	}

	Ref<Window> WindowService::CreateWindow(const WindowCreateParams& createParams)
	{
		WindowingPlatform* windowingPlatform = dynamic_cast<WindowingPlatform*>(&Engine::Get()->GetPlatform());
		CocoAssert(windowingPlatform, "Platform does not support windowing")

		if (_windows.size() > 0 && !windowingPlatform->SupportsMultipleWindows())
		{
			CocoError("Platform does not support multiple windows")
			return Ref<Window>();
		}

		auto& window = _windows.emplace_back(windowingPlatform->CreatePlatformWindow(createParams));

		return window;
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
		WindowingPlatform* windowingPlatform = dynamic_cast<WindowingPlatform*>(&Engine::Get()->GetPlatform());
		Assert(windowingPlatform)

		return windowingPlatform->GetDisplays();
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