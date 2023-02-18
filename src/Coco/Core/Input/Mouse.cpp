#include "Mouse.h"

namespace Coco::Input
{
	MouseStateChange MouseStateChange::MoveStateChange(const Vector2Int& newPosition, const Vector2Int& delta)
	{
		MouseStateChange state = {};
		state.Position = newPosition;
		state.MoveDelta = delta;
		return state;
	}

	MouseStateChange MouseStateChange::ScrollStateChange(const Vector2Int& scrollDelta)
	{
		MouseStateChange state = {};
		state.ScrollDelta = scrollDelta;
		return state;
	}

	MouseStateChange MouseStateChange::ButtonStateChange(int buttonIndex, bool isPressed)
	{
		MouseStateChange state = {};
		state.ButtonIndex = buttonIndex;
		state.IsButtonPressed = isPressed;
		return state;
	}

	void Mouse::UpdateButtonState(Button button, bool isPressed)
	{
		int index = static_cast<int>(button);
		if (_preProcessState.ButtonStates[index] == isPressed)
			return;

		_preProcessStateChanges.Add(MouseStateChange::ButtonStateChange(index, isPressed));
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
		_preProcessState.ScrollDelta = scrollDelta;
	}

	void Mouse::DoubleClicked(Button button)
	{
		OnDoubleClicked.InvokeEvent(button);
	}

	bool Mouse::IsButtonPressed(Button button) const
	{
		return _currentState.ButtonStates[static_cast<int>(button)];
	}

	bool Mouse::WasButtonJustPressed(Button button) const
	{
		int index = static_cast<int>(button);
		return (_currentState.ButtonStates[index] && !_previousState.ButtonStates[index]);
	}

	bool Mouse::WasButtonJustReleased(Button button) const
	{
		int index = static_cast<int>(button);
		return (!_currentState.ButtonStates[index] && _previousState.ButtonStates[index]);
	}

	void Mouse::ProcessCurrentState()
	{
		// Step through each state change since the last tick and fire the proper events
		for(const auto& newState : _preProcessStateChanges)
		{
			if (newState.ButtonIndex.has_value())
			{
				if (newState.IsButtonPressed)
				{
					OnButtonPressed.InvokeEvent(static_cast<Mouse::Button>(newState.ButtonIndex.value()));
				}
				else
				{
					OnButtonReleased.InvokeEvent(static_cast<Mouse::Button>(newState.ButtonIndex.value()));
				}
			}
			else if (newState.Position.has_value())
			{
				OnMoved.InvokeEvent(newState.Position.value(), newState.MoveDelta);
			}
			else if (newState.ScrollDelta.has_value())
			{
				OnScrolled.InvokeEvent(newState.ScrollDelta.value());
			}
		}

		_currentState = _preProcessState;
		_preProcessStateChanges.Clear();
	}

	void Mouse::SavePreviousState()
	{
		_previousState = _currentState;
		_currentState.ScrollDelta = Vector2Int::Zero;
		_preProcessState = _currentState;
	}
}