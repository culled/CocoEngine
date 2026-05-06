//
// Created by cullen on 3/1/26.
//

#include "InputEvent.h"

namespace Coco
{
    ButtonInputEvent::ButtonInputEvent(bool isPressed) noexcept :
        IsPressed(isPressed)
    {}
} // Coco