#include "Mouse.h"

namespace Coco::Input
{
	MouseStateChange MouseStateChange::MoveStateChange(const Vector2Int& newPosition, const Vector2Int& delta) noexcept
	{
		MouseStateChange state = {};
		state.Position = newPosition;
		state.MoveDelta = delta;
		return state;
	}

	MouseStateChange MouseStateChange::ScrollStateChange(const Vector2Int& scrollDelta) noexcept
	{
		MouseStateChange state = {};
		state.ScrollDelta = scrollDelta;
		return state;
	}

	MouseStateChange MouseStateChange::ButtonStateChange(MouseButton button, bool isPressed) noexcept
	{
		MouseStateChange state = {};
		state.Button = button;
		state.IsButtonPressed = isPressed;
		return state;
	}

	void Mouse::UpdateButtonState(MouseButton button, bool isPressed)
	{
		const int index = static_cast<int>(button);
		if (_preProcessState.ButtonStates[index] == isPressed)
			return;

		_preProcessStateChanges.Add(MouseStateChange::ButtonStateChange(button, isPressed));
		_preProcessState.ButtonStates[index] = isPressed;
	}

	void Mouse::UpdatePositionState(const Vector2Int& newPosition)
	{
		if (_preProcessState.Position == newPosition)
			return;

		_preProcessStateChanges.Add(MouseStateChange::MoveStateChange(newPosition, newPosition - _preProcessState.Position));
		_preProcessState.Position = newPosition;
	}

	void Mouse::UpdateScrollState(const Vector2Int& scrollDelta)
	{
		if (scrollDelta == Vector2Int::Zero)
			return;

		_preProcessStateChanges.Add(MouseStateChange::ScrollStateChange(scrollDelta));
	}

	void Mouse::DoubleClicked(MouseButton button)
	{
		OnDoubleClicked.Invoke(button);
	}

	bool Mouse::IsButtonPressed(MouseButton button) const noexcept
	{
		return _currentState.ButtonStates[static_cast<int>(button)];
	}

	bool Mouse::WasButtonJustPressed(MouseButton button) const noexcept
	{
		const int index = static_cast<int>(button);
		return (_currentState.ButtonStates[index] && !_previousState.ButtonStates[index]);
	}

	bool Mouse::WasButtonJustReleased(MouseButton button) const noexcept
	{
		const int index = static_cast<int>(button);
		return (!_currentState.ButtonStates[index] && _previousState.ButtonStates[index]);
	}

	void Mouse::ProcessCurrentState()
	{
		if (_isFirstState)
		{
			// Set the previous state to the current to prevent large deltas on the first frame
			_previousState = _preProcessState;
		}
		else
		{
			// Step through each state change since the last tick and fire the proper events
			for (const auto& newState : _preProcessStateChanges)
			{
				if (newState.Button.has_value())
				{
					if (newState.IsButtonPressed)
					{
						OnButtonPressed.Invoke(newState.Button.value());
					}
					else
					{
						OnButtonReleased.Invoke(newState.Button.value());
					}
				}
				else if (newState.Position.has_value())
				{
					OnMoved.Invoke(newState.Position.value(), newState.MoveDelta);
				}
				else if (newState.ScrollDelta.has_value())
				{
					OnScrolled.Invoke(newState.ScrollDelta.value());

					// Make sure we preserve a scroll for the tick (else GetScrollWheelDelta() will always be 0) 
					_preProcessState.ScrollDelta = newState.ScrollDelta.value();
				}
			}
		}

		_currentState = _preProcessState;
		_preProcessStateChanges.Clear();
		_isFirstState = false;
	}

	void Mouse::SavePreviousState() noexcept
	{
		_previousState = _currentState;
		_currentState.ScrollDelta = Vector2Int::Zero;
		_preProcessState = _currentState;
	}
}