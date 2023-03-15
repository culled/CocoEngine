#include "WindowingService.h"

#include <Coco/Core/Application.h>
#include <Coco/Core/Engine.h>
#include <Coco/Core/Platform/IEnginePlatform.h>
#include <Coco/Rendering/RenderingService.h>

#include "IWindowingPlatform.h"
#include "Window.h"

namespace Coco::Windowing
{
	WindowingService::WindowingService() noexcept :
		_mainWindow(nullptr), _renderTickListener(CreateRef<MainLoopTickListener>(this, &WindowingService::RenderTick, WindowRenderPriority))
	{}

	WindowingService::~WindowingService()
	{
		Engine::Get()->GetMainLoop()->RemoveTickListener(_renderTickListener);
		_mainWindow = nullptr;
		_windows.Clear();
	}

	Logging::Logger* WindowingService::GetLogger() const noexcept
	{
		return Engine::Get()->GetLogger();
	}

	void WindowingService::Start()
	{
		if (!Engine::Get()->GetServiceManager()->TryFindService<Rendering::RenderingService>(_renderingService))
			throw Exception("Could not find an active rendering service. The windowing service requires an active rendering service");

		Engine::Get()->GetMainLoop()->AddTickListener(_renderTickListener);
	}

	Window* WindowingService::CreateNewWindow(WindowCreateParameters& createParameters)
	{
		if (Platform::IWindowingPlatform* platform = dynamic_cast<Platform::IWindowingPlatform*>(Engine::Get()->GetPlatform()))
		{
			_windows.Add(platform->CreatePlatformWindow(createParameters, this));
			const Managed<Window>& window = _windows.Last();
			Window* windowPtr = window.get();

			if (_mainWindow == nullptr)
				_mainWindow = windowPtr;

			return windowPtr;
		}

		throw Exception("Current platform does not support windows");
	}

	bool WindowingService::TryFindWindow(void* windowId, Window** window) const noexcept
	{
		const auto it = std::find_if(_windows.cbegin(), _windows.cend(), [windowId](const Managed<Window>& other) {
			return windowId == other->GetID();
			});

		if (it != _windows.cend())
		{
			*window = (*it).get();
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
			Engine::Get()->GetApplication()->Quit();
			return;
		}

		auto it = std::find_if(_windows.begin(), _windows.end(), [window](const Managed<Window>& other) noexcept {
			return window == other.get();
			});

		try
		{
			if (it != _windows.end())
				_windows.Erase(it);
		}
		catch(...)
		{ }
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

			// TODO: which camera should each window use?
			_renderingService->Render(window->GetPresenter());
		}
	}
}