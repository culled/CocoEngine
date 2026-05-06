//
// Created by cullen on 3/1/26.
//

#ifndef COCOENGINE_MOUSE_H
#define COCOENGINE_MOUSE_H
#include <Coco/Core/Types/CoreTypes.h>
#include <Coco/Core/Math/Vector2.h>

#include "MouseTypes.h"

namespace Coco
{
    /// @brief A state for a Mouse
    struct MouseState
    {
        /// @brief The maximum number of Mouse buttons
        static constexpr uint8 ButtonCount = 6;

        /// @brief Mouse button states
        bool ButtonStates[ButtonCount];

        /// @brief The Mouse position
        Vector2i Position;

        /// @brief The movement delta since the last tick
        Vector2i MoveDelta;

        /// @brief The scroll delta since the last tick
        Vector2i ScrollDelta;

        Vector2i RawMoveDelta;

        MouseState() noexcept;
    };

    class InputService;

    class Mouse
    {
        friend class InputService;

    public:
        Mouse(InputService* inputService);

        /// @brief Adds a button state change event for a Mouse button
        /// @param button The button
        /// @param isPressed The pressed state for the button
        void AddButtonStateChangeEvent(MouseButton button, bool isPressed);

        /// @brief Adds a position change event for the Mouse
        /// @param newPosition The new Mouse position
        void AddPositionChangedEvent(const Vector2i& newPosition);

        /// @brief Adds a scroll event for the Mouse
        /// @param scrollDelta The scroll delta
        void AddScrollEvent(const Vector2i& scrollDelta);

        void AddRawMoveEvent(const Vector2i& moveDelta);

        /// @brief Determines if the given Mouse button was pressed in the last tick
        /// @param button The Mouse button
        /// @return True if the Mouse button transitioned from released->pressed in the last tick
        bool WasButtonJustPressed(MouseButton button) const;

        /// @brief Determines if the given Mouse button was released in the last tick
        /// @param button The Mouse button
        /// @return True if the Mouse button transitioned from pressed->released in the last tick
        bool WasButtonJustReleased(MouseButton button) const;

        /// @brief Determines if the given Mouse button is currently pressed
        /// @param button The Mouse button
        /// @return True if the Mouse button is currently pressed
        bool IsButtonPressed(MouseButton button) const;

        /// @brief Gets the current position of the Mouse
        /// @return The current Mouse position
        const Vector2i& GetPosition() const { return _currentState.Position; }

        /// @brief Gets the movement delta since the last tick
        /// @return The movement delta since the last tick
        const Vector2i& GetMoveDelta() const { return _currentState.MoveDelta; }

        /// @brief Gets the scroll delta since the last tick
        /// @return The scroll delta since the last tick
        const Vector2i& GetScrollDelta() const { return _currentState.ScrollDelta; }

        const Vector2i& GetRawMoveDelta() const { return _currentState.RawMoveDelta; }

    private:
        InputService* _inputService;
        MouseState _currentState;
        MouseState _previousState;

    private:
        /// @brief Saves the previous input state
        void SavePreviousState();

        /// @brief Resets the state of all Mouse buttons
        void ResetState();
    };
} // Coco

#endif //COCOENGINE_MOUSE_H