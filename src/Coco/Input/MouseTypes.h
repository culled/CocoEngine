//
// Created by cullen on 3/1/26.
//

#ifndef COCOENGINE_MOUSETYPES_H
#define COCOENGINE_MOUSETYPES_H
#include "InputEvent.h"
#include <Coco/Core/Math/Vector2.h>

namespace Coco
{
    /// @brief Mouse buttons
    enum class MouseButton : uint8
    {
        Left = 0,
        Middle,
        Right,
        Button3,
        Button4,
        Unknown
    };

    const char* ToString(MouseButton button);

    /// @brief Data for an event fired when a mouse button is pressed/released
    struct MouseButtonInputEvent :
        public ButtonInputEvent
    {
        static constexpr uint64 Type = 1;

        /// @brief The mouse button
        MouseButton Button;

        MouseButtonInputEvent(MouseButton button, bool isPressed) noexcept;

        // Inherited via ButtonInputEvent
        uint64 GetType() const noexcept override { return Type; }
    };

    /// @brief Data for an event fired when a mouse moves
    struct MouseMoveInputEvent :
        public InputEvent
    {
        static constexpr uint64 Type = 2;

        /// @brief The new position of the mouse
        Vector2i NewPosition;

        /// @brief The movement of the mouse since the last frame
        Vector2i Delta;

        MouseMoveInputEvent(const Vector2i& newPosition, const Vector2i& delta) noexcept;

        uint64 GetType() const noexcept override { return Type; }
    };

    /// @brief Data for an event fired when a mouse scrolls
    struct MouseScrollInputEvent :
        public InputEvent
    {
        static constexpr uint64 Type = 3;

        /// @brief The movement of the mouse wheel since the last frame
        Vector2i Delta;

        MouseScrollInputEvent(const Vector2i& delta) noexcept;

        uint64 GetType() const noexcept override { return Type; }
    };

    /// @brief Data for an event fired when raw mouse movement is received
    struct MouseRawMoveInputEvent :
        public InputEvent
    {
        static constexpr uint64 Type = 4;

        /// @brief The movement of the mouse since the last frame
        Vector2i Delta;

        MouseRawMoveInputEvent(const Vector2i& delta) noexcept;

        uint64 GetType() const noexcept override { return Type; }
    };
}
#endif //COCOENGINE_MOUSETYPES_H