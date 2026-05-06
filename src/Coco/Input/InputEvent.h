//
// Created by cullen on 3/1/26.
//

#ifndef COCOENGINE_INPUTEVENT_H
#define COCOENGINE_INPUTEVENT_H
#include <Coco/Core/Types/CoreTypes.h>

namespace Coco
{
    /// @brief Base class for an input event
    struct InputEvent
    {
        virtual ~InputEvent() noexcept = default;

        /// @brief Gets the type of event this event is
        /// @return The event type
        virtual uint64 GetType() const noexcept = 0;
    };

    /// @brief Base class for an InputEvent where a button is pressed/released
    struct ButtonInputEvent :
        public InputEvent
    {
        /// @brief The pressed state of the button
        bool IsPressed;

        ButtonInputEvent(bool isPressed) noexcept;
        virtual ~ButtonInputEvent() noexcept = default;
    };
} // Coco

#endif //COCOENGINE_INPUTEVENT_H