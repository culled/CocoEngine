//
// Created by cullen on 3/1/26.
//

#include "Mouse.h"
#include "InputService.h"
#include "Coco/Core/Engine.h"

namespace Coco
{
    MouseState::MouseState() noexcept :
        ButtonStates{false},
        Position(),
        MoveDelta(),
        ScrollDelta(),
        RawMoveDelta()
    {}

    Mouse::Mouse(InputService* inputService) :
        _inputService(inputService),
        _currentState(),
        _previousState()
    {}

    void Mouse::AddButtonStateChangeEvent(MouseButton button, bool isPressed)
    {
        COCO_ASSERT(button <= MouseButton::Unknown, "Invalid mouse button");
        int buttonIndex = static_cast<int>(button);

        if (_currentState.ButtonStates[buttonIndex] != isPressed)
        {
            _currentState.ButtonStates[buttonIndex] = isPressed;
            _inputService->DispatchInputEvent(MouseButtonInputEvent(button, isPressed));
        }
    }

    void Mouse::AddPositionChangedEvent(const Vector2i& newPosition)
    {
        if (_currentState.Position == newPosition)
            return;

        _currentState.MoveDelta = newPosition - _currentState.Position;
        _currentState.Position = newPosition;
        _inputService->DispatchInputEvent(MouseMoveInputEvent(_currentState.Position, _currentState.MoveDelta));
    }

    void Mouse::AddScrollEvent(const Vector2i& scrollDelta)
    {
        if (scrollDelta == Vector2i::Zero)
            return;

        _currentState.ScrollDelta = scrollDelta;
        _inputService->DispatchInputEvent(MouseScrollInputEvent(_currentState.ScrollDelta));
    }

    void Mouse::AddRawMoveEvent(const Vector2i& moveDelta)
    {
        _currentState.RawMoveDelta = moveDelta;
        _inputService->DispatchInputEvent(MouseRawMoveInputEvent(_currentState.RawMoveDelta));
    }

    bool Mouse::WasButtonJustPressed(MouseButton button) const
    {
        COCO_ASSERT(button <= MouseButton::Unknown, "Invalid mouse button");
        int buttonIndex = static_cast<int>(button);

        return _currentState.ButtonStates[buttonIndex] && !_previousState.ButtonStates[buttonIndex];
    }

    bool Mouse::WasButtonJustReleased(MouseButton button) const
    {
        COCO_ASSERT(button <= MouseButton::Unknown, "Invalid mouse button");
        int buttonIndex = static_cast<int>(button);

        return _previousState.ButtonStates[buttonIndex] && !_currentState.ButtonStates[buttonIndex];
    }

    bool Mouse::IsButtonPressed(MouseButton button) const
    {
        COCO_ASSERT(button <= MouseButton::Unknown, "Invalid mouse button");
        int buttonIndex = static_cast<int>(button);

        return _currentState.ButtonStates[buttonIndex];
    }

    void Mouse::SavePreviousState()
    {
        _previousState = _currentState;
        _currentState.MoveDelta = Vector2i::Zero;
        _currentState.ScrollDelta = Vector2i::Zero;
        _currentState.RawMoveDelta = Vector2i::Zero;
    }

    void Mouse::ResetState()
    {
        for (uint8 i = 0; i < MouseState::ButtonCount; i++)
        {
            if (_currentState.ButtonStates[i])
                AddButtonStateChangeEvent(static_cast<MouseButton>(i), false);
        }
    }
} // Coco