#include "Window.h"

#include <Coco/Core/Engine.h>
#include <Coco/Core/Services/EngineServiceManager.h>
#include <Coco/Rendering/RenderingService.h>
#include "WindowingService.h"

namespace Coco::Windowing
{
	Window::Window(Windowing::WindowingService* windowingService) :
		WindowingService(windowingService)
	{
		Rendering::RenderingService* renderingService;
		if (!Engine::Get()->GetServiceManager()->TryFindService<Rendering::RenderingService>(renderingService))
			throw Exception("Could not find an active rendering service. Windowing requires an active rendering service");

		Presenter = renderingService->CreatePresenter();
	}

	Window::~Window()
	{
		Presenter.reset();
	}

	bool Window::Close() noexcept
	{
		bool cancelClose = false;

		try
		{
			OnClosing.Invoke(this, cancelClose);
		}
		catch(...)
		{ }

		if (cancelClose)
			return false;

		try
		{
			OnClosed.Invoke(this);
		}
		catch (...)
		{ }

		WindowingService->WindowClosed(this);

		return true;
	}

	void Window::HandleResized()
	{
		// By handling the surface on resize, we can make sure we always have a surface
		if (!Presenter->IsSurfaceInitialized())
			SetupPresenterSurface();

		Presenter->SetBackbufferSize(GetBackbufferSize());

		OnResized.Invoke(this, GetSize());
	}
}