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

		if (!Engine::Get()->GetServiceManager()->TryFindService<Rendering::RenderingService>(&renderingService))
			throw Exception("Could not find an active rendering service. Windowing requires an active rendering service");

		Presenter = renderingService->CreatePresenter();
	}

	Window::~Window()
	{
		Presenter.reset();
	}

	bool Window::Close()
	{
		bool cancelClose = false;
		OnClosing.InvokeEvent(this, cancelClose);

		if (cancelClose)
			return false;

		OnClosed.InvokeEvent(this);
		WindowingService->WindowClosed(this);

		return true;
	}

	void Window::HandleResized()
	{
		// By handling the surface on resize, we can make sure we always have a surface
		if (!Presenter->IsSurfaceInitialized())
			SetupPresenterSurface();

		Presenter->SetBackbufferSize(GetBackbufferSize());

		OnResized.InvokeEvent(this, GetSize());
	}
}