#include "Windowpch.h"
#include "Window.h"

#include "WindowService.h"
#include <Coco/Core/Engine.h>
#include <Coco/Rendering/Graphics/Presenter.h>
#include <Coco/Rendering/RenderService.h>

namespace Coco::Windowing
{
	const WindowID Window::InvalidID = Math::MaxValue<WindowID>();
	const uint16 Window::DefaultDPI = 96;

	Window::Window(const WindowCreateParams& createParams) :
		ID(_id++),
		_parentID(createParams.ParentWindow),
		_presenter()
	{
		using namespace Coco::Rendering;

		RenderService* rendering = RenderService::Get();
		CocoAssert(rendering, "Windowing depends on the RenderService")

		_presenter = rendering->GetDevice().CreatePresenter();
	}

	Window::~Window()
	{
		try
		{
			OnClosed.Invoke();
		}
		catch(...)
		{ }

		if (_presenter.IsValid())
		{
			using namespace Coco::Rendering;

			RenderService* rendering = RenderService::Get();
			CocoAssert(rendering, "Windowing depends on the RenderService")

			rendering->GetDevice().TryReleaseResource(_presenter->ID);
		}
	}

	void Window::Close()
	{
		bool cancel = false;

		OnClosing.Invoke(cancel);

		if (!cancel)
		{
			WindowService* windowing = WindowService::Get();
			CocoAssert(windowing, "WindowService singleton was null")

			windowing->WindowClosed(*this);
		}
	}

	bool Window::HasParent() const
	{
		return _parentID != InvalidID;
	}

	WindowID Window::GetParentID() const
	{
		return _parentID;
	}

	RectInt Window::GetRect(bool clientArea, bool relativeToParent) const
	{
		return RectInt(GetPosition(clientArea, relativeToParent), clientArea ? GetClientAreaSize() : GetSize());
	}

	Ref<Window> Window::GetParentWindow() const
	{
		WindowService* windowing = WindowService::Get();
		CocoAssert(windowing, "WindowService singleton was null")

		return windowing->GetWindow(_parentID);
	}

	void Window::HandleResized()
	{
		OnResized.Invoke(GetClientAreaSize());

		// Lazy initialize surface until we're visible
		EnsurePresenterSurface();

		_presenter->SetFramebufferSize(GetClientAreaSize());
	}

	void Window::EnsurePresenterSurface()
	{
		if (_presenter->HasSurface())
			return;

		_presenter->SetSurface(CreateSurface());

		_presenter->SetFramebufferSize(GetClientAreaSize());
	}

	std::atomic<WindowID> Window::_id;
}