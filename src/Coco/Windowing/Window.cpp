#include "Window.h"

#include "WindowingService.h"

namespace Coco::Windowing
{
	Window::Window(Windowing::WindowingService* windowingService) :
		WindowingService(windowingService)
	{
	}

	Window::~Window()
	{
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
}