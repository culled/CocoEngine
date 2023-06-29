#include "WindowingService.h"

#include <Coco/Core/Application.h>
#include <Coco/Core/Engine.h>
#include <Coco/Core/Platform/IEnginePlatform.h>
#include "IWindowingPlatform.h"

namespace Coco::Windowing
{
	WindowingService::WindowingService(EngineServiceManager* serviceManager) : EngineService(serviceManager)
	{}

	WindowingService::~WindowingService()
	{
		_windows.Clear();
	}

	Ref<Window> WindowingService::CreateNewWindow(const WindowCreateParameters& createParameters)
	{
		if (Platform::IWindowingPlatform* platform = dynamic_cast<Platform::IWindowingPlatform*>(ServiceManager->Engine->GetPlatform()))
		{
			_windows.Add(platform->CreatePlatformWindow(createParameters, this));
			const ManagedRef<Window>& window = _windows.Last();

			if (!_mainWindow.IsValid())
				_mainWindow = window;

			return window;
		}

		throw WindowCreateException("Current platform does not support windows");
	}

	bool WindowingService::TryFindWindow(void* windowId, Ref<Window>& window) const noexcept
	{
		const auto it = _windows.Find([windowId](const ManagedRef<Window>& other) noexcept {
			return windowId == other->GetID();
			});

		if (it != _windows.cend())
		{
			window = *it;
			return true;
		}

		return false;
	}

	List<Ref<Window>> WindowingService::GetRenderableWindows() const noexcept
	{
		List<Ref<Window>> renderableWindows;

		for (const ManagedRef<Window>& window : _windows)
		{
			// Don't render invisble windows
			if (!window->GetIsVisible())
				continue;

			renderableWindows.Add(window);
		}

		return renderableWindows;
	}

	void WindowingService::WindowClosed(Window* window) noexcept
	{
		// The main window closes with the application
		if (window == _mainWindow.Get())
		{
			ServiceManager->Engine->GetApplication()->Quit();
			return;
		}

		auto it = _windows.Find([window](const ManagedRef<Window>& other) noexcept {
			return window == other.Get();
			});

		if (it != _windows.end())
			_windows.Remove(it);
	}
}