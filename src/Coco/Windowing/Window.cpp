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
		// TODO: cancelling close

		WindowingService->WindowClosed(this);
		return true;
	}
}