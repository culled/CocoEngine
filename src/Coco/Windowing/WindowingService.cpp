#include "WindowingService.h"

#include <Coco/Core/Application.h>
#include <Coco/Core/Engine.h>
#include <Coco/Core/Platform/IEnginePlatform.h>
#include <Coco/Rendering/RenderingService.h>

#include "IWindowingPlatform.h"
#include "Window.h"

namespace Coco::Windowing
{
	WindowingService::WindowingService(Coco::Engine* engine) : EngineService(engine),
		_mainWindow(nullptr)
	{
		RegisterTickListener(this, &WindowingService::RenderTick, WindowRenderPriority);
	}

	WindowingService::~WindowingService()
	{
		_mainWindow = nullptr;
		_windows.Clear();
	}

	void WindowingService::StartImpl()
	{
		if (!this->Engine->GetServiceManager()->TryFindService<Rendering::RenderingService>(_renderingService))
			throw EngineServiceStartException("Could not find an active rendering service. The windowing service requires an active rendering service");
	}

	Window* WindowingService::CreateNewWindow(WindowCreateParameters& createParameters)
	{
		if (Platform::IWindowingPlatform* platform = dynamic_cast<Platform::IWindowingPlatform*>(this->Engine->GetPlatform()))
		{
			_windows.Add(platform->CreatePlatformWindow(createParameters, this));
			const Managed<Window>& window = _windows.Last();
			Window* windowPtr = window.get();

			if (_mainWindow == nullptr)
				_mainWindow = windowPtr;

			return windowPtr;
		}

		throw WindowCreateException("Current platform does not support windows");
	}

	bool WindowingService::TryFindWindow(void* windowId, Window*& window) const noexcept
	{
		const auto it = _windows.Find([windowId](const Managed<Window>& other) noexcept {
			return windowId == other->GetID();
			});

		if (it != _windows.cend())
		{
			window = (*it).get();
			return true;
		}

		window = nullptr;
		return false;
	}

	void WindowingService::WindowClosed(Window* window) noexcept
	{
		// The main window closes with the application
		if (window == _mainWindow)
		{
			this->Engine->GetApplication()->Quit();
			return;
		}

		auto it = _windows.Find([window](const Managed<Window>& other) noexcept {
			return window == other.get();
			});

		if (it != _windows.end())
			_windows.Remove(it);
	}

	void WindowingService::RenderTick(double deltaTime)
	{
		if (!_renderingService)
			return;

		for (const Managed<Window>& window : _windows)
		{
			// Don't render invisble windows
			if (!window->GetIsVisible())
				continue;

			// TODO: which camera and render pipeline should each window use?
			_renderingService->Render(window->GetPresenter());
		}
	}
}