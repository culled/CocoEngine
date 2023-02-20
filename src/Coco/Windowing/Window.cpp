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
			throw Exception("Windowing requires an active rendering service");

		Presenter.reset(renderingService->CreatePresenter());
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

		// The windowing service will handle the OnClosed event
		WindowingService->WindowClosed(this);

		return true;
	}

	void Window::OnResized()
	{
		if (!Presenter->IsSurfaceInitialized())
			SetupPresenterSurface();

		Presenter->SetBackbufferSize(GetBackbufferSize());
	}
}