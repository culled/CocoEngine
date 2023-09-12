#include "Windowpch.h"
#include "Window.h"

#include <Coco/Windowing/WindowService.h>
#include <Coco/Core/Engine.h>

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
		_parentID(createParams.ParentWindow)
	{}

	Window::~Window()
	{
		try
		{
			OnClosed.Invoke();
		}
		catch(...)
		{ }
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

	Window* Window::GetParentWindow()
	{
		return WindowService::Get()->GetWindow(_parentID);
	}

	const Window* Window::GetParentWindow() const
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
	}
}