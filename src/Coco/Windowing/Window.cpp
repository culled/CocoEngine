//
// Created by cullen on 2/28/26.
//

#include "Window.h"

#include "WindowService.h"
#include "Coco/Rendering/RenderService.h"

namespace Coco
{
    WindowCreateParams::WindowCreateParams(const char* title, const Sizei& initialSize) :
        Title(title),
        InitialSize(initialSize),
        IsFullscreen(false),
        CanResize(true),
        FocusWhenShown(true),
        InitialState(WindowState::Default),
        StyleFlags(WindowStyleFlags::None),
        InitialPosition(),
        InitialDisplayIndex(),
        ParentWindow()
    {}

    void Window::RequestClose()
    {
        bool cancel = false;
        OnCloseRequested.Invoke(cancel);

        if (!cancel)
            Close();
    }

    Window::Window(uint32 id, bool isMainWindow, Ref<Window> parentWindow) :
        _id(id),
        _parent(std::move(parentWindow)),
        _isMainWindow(isMainWindow)
    {}
} // Coco