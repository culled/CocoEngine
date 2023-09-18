#include "Windowpch.h"
#include "Window.h"

#include "WindowService.h"
#include <Coco/Core/Engine.h>
#include <Coco/Rendering/RenderService.h>

namespace Coco::Windowing
{
	WindowCreateParams::WindowCreateParams(const char* title, const SizeInt& initialSize) :
		Title(title),
		InitialSize(initialSize),
		CanResize(true),
		InitialState(WindowState::Default),
		IsFullscreen(false),
		InitialPosition(),
		ParentWindow(Window::InvalidID),
		DisplayIndex()
	{}

	std::atomic<WindowID> Window::_id;

	Window::Window(const WindowCreateParams& createParams) :
		ID(_id++),
		_parentID(createParams.ParentWindow),
		_presenter()
	{
		if (!Rendering::RenderService::Get())
			throw std::exception("No RenderService is active");

		_presenter = Rendering::RenderService::Get()->GetDevice()->CreatePresenter();
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
			_presenter.Invalidate();
			Rendering::RenderService::Get()->GetDevice()->PurgeUnusedResources();
		}
	}

	void Window::Close()
	{
		bool cancel = false;

		try
		{
			OnClosing.Invoke(cancel);
		}
		catch (const std::exception& ex)
		{
			CocoError("Error while invoking Window::OnClosing event: {}", ex.what())
		}

		if (!cancel)
		{
			Windowing::WindowService::Get()->WindowClosed(*this);
		}
	}

	WindowID Window::GetParentID() const
	{
		return _parentID;
	}

	Ref<Window> Window::GetParentWindow() const
	{
		return WindowService::Get()->GetWindow(_parentID);
	}

	void Window::HandleResized()
	{
		try
		{
			OnResized.Invoke(GetClientAreaSize());
		}
		catch(const std::exception& ex)
		{
			CocoError("Error while invoking Window::OnResized event: {}", ex.what())
		}

		// Lazy initialize surface until we're visible
		if (!_presenter->SurfaceInitialized())
		{
			_presenter->InitializeSurface(CreateSurface());
		}

		_presenter->SetFramebufferSize(GetClientAreaSize());
	}
}