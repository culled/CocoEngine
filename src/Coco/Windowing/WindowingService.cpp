#include "WindowingService.h"

#include <Coco/Core/Application.h>
#include <Coco/Core/Engine.h>
#include <Coco/Core/Platform/EnginePlatform.h>

#include "WindowingPlatform.h"
#include "Window.h"

namespace Coco::Windowing
{
	WindowingService::WindowingService() :
		_mainWindow(nullptr)
	{
	}

	WindowingService::~WindowingService()
	{
		_mainWindow = nullptr;
		_windows.Clear();
	}

	Logging::Logger* WindowingService::GetLogger() const
	{
		return Engine::Get()->GetLogger();
	}

	void WindowingService::Start()
	{
	}

	Window* WindowingService::CreateNewWindow(WindowCreateParameters& createParameters)
	{
		if (Platform::WindowingPlatform* platform = dynamic_cast<Platform::WindowingPlatform*>(Engine::Get()->GetPlatform()))
		{
			Window* window = platform->CreatePlatformWindow(createParameters, this);

			if (_mainWindow == nullptr)
				_mainWindow = window;

			_windows.Add(Managed<Window>(window));

			return window;
		}

		throw Exception("Current platform does not support windows");
	}

	bool WindowingService::TryFindWindow(void* windowId, Window** window) const
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

	void WindowingService::WindowClosed(Window* window)
	{
		// The main window closes with the application
		if (window == _mainWindow)
		{
			if (Engine::Get()->GetApplication()->Quit())
				window->OnClosed.InvokeEvent(window);

			return;
		}

		window->OnClosed.InvokeEvent(window);

		auto it = std::find_if(_windows.begin(), _windows.end(), [window](const Managed<Window>& other) {
			return window == other.get();
			});

		if (it != _windows.end())
		{
			_windows.Erase(it);
		}
	}
}