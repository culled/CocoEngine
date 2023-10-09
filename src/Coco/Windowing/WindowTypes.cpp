#include "Windowpch.h"
#include "WindowTypes.h"

#include "Window.h"

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
		DisplayIndex(),
		StyleFlags(WindowStyleFlags::None)
	{}
}