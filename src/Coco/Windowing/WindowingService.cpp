#include "WindowingService.h"

#include <Coco/Core/Application.h>
#include <Coco/Core/Engine.h>
#include <Coco/Core/Platform/IEnginePlatform.h>
#include <Coco/Rendering/RenderingService.h>
#include "IWindowingPlatform.h"

namespace Coco::Windowing
{
	WindowingService::WindowingService(Coco::Engine* engine) : EngineService(engine)
	{
		RegisterTickListener(this, &WindowingService::RenderTick, WindowRenderPriority);
	}

	WindowingService::~WindowingService()
	{
		_mainWindow.Invalidate();
		_windows.Clear();
	}

	void WindowingService::StartImpl()
	{
		if (!this->Engine->GetServiceManager()->TryFindService<Rendering::RenderingService>(_renderingService))
			throw EngineServiceStartException("Could not find an active rendering service. The windowing service requires an active rendering service");
	}

	WeakManagedRef<Window> WindowingService::CreateNewWindow(WindowCreateParameters& createParameters)
	{
		if (Platform::IWindowingPlatform* platform = dynamic_cast<Platform::IWindowingPlatform*>(this->Engine->GetPlatform()))
		{
			_windows.Add(platform->CreatePlatformWindow(createParameters, this));
			const ManagedRef<Window>& window = _windows.Last();

			if (!_mainWindow.IsValid())
				_mainWindow = window;

			return window;
		}

		throw WindowCreateException("Current platform does not support windows");
	}

	bool WindowingService::TryFindWindow(void* windowId, WeakManagedRef<Window>& window) const noexcept
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

	void WindowingService::WindowClosed(Window* window) noexcept
	{
		// The main window closes with the application
		if (window == _mainWindow.Get())
		{
			this->Engine->GetApplication()->Quit();
			return;
		}

		auto it = _windows.Find([window](const ManagedRef<Window>& other) noexcept {
			return window == other.Get();
			});

		if (it != _windows.end())
			_windows.Remove(it);
	}

	void WindowingService::RenderTick(double deltaTime)
	{
		if (!_renderingService)
			return;

		for (const ManagedRef<Window>& window : _windows)
		{
			// Don't render invisble windows
			if (!window->GetIsVisible())
				continue;

			// TODO: which camera and render pipeline should each window use?
			_renderingService->Render(window->GetPresenter());
		}
	}
}