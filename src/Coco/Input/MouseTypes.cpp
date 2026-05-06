//
// Created by cullen on 3/1/26.
//
#include "MouseTypes.h"

namespace Coco
{
    const char* ToString(MouseButton button)
    {
        switch (button)
        {
            case MouseButton::Left:
                return "Left";
            case MouseButton::Middle:
                return "Middle";
            case MouseButton::Right:
                return "Right";
            case MouseButton::Button3:
                return "Button 3";
            case MouseButton::Button4:
                return "Button 4";
            default:
                return "Unknown";
        }

    }

    MouseButtonInputEvent::MouseButtonInputEvent(MouseButton button, bool isPressed) noexcept :
        ButtonInputEvent(isPressed),
        Button(button)
    {}

    MouseMoveInputEvent::MouseMoveInputEvent(const Vector2i& newPosition, const Vector2i& delta) noexcept :
        NewPosition(newPosition),
        Delta(delta)
    {}

    MouseScrollInputEvent::MouseScrollInputEvent(const Vector2i& delta) noexcept :
        Delta(delta)
    {}

    MouseRawMoveInputEvent::MouseRawMoveInputEvent(const Vector2i& delta) noexcept :
        Delta(delta)
    {}
}
