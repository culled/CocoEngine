#include "Window.h"

#include <Coco/Core/Engine.h>
#include "WindowingService.h"

namespace Coco::Windowing
{
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

		WindowingService::Get()->WindowClosed(this);

		return true;
	}

	void Window::HandleResized()
	{
		// By handling the surface on resize, we can make sure we always have a surface
		if (!_presenter->IsSurfaceInitialized())
			SetupPresenterSurface();

		_presenter->SetBackbufferSize(GetBackbufferSize());

		OnResized.Invoke(this, GetSize());
	}
}