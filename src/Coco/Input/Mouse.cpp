#include "Inputpch.h"
#include "Mouse.h"

namespace Coco::Input
{
	MouseState::MouseState() :
		ButtonStates{false},
		Position(Vector2Int::Zero),
		ScrollDelta(Vector2Int::Zero)
	{}

	MouseStateChange::MouseStateChange() :
		Button(),
		IsButtonPressed(false),
		Position(),
		MoveDelta(Vector2Int::Zero),
		ScrollDelta()
	{}

	MouseStateChange MouseStateChange::MoveStateChange(const Vector2Int& newPosition, const Vector2Int& delta)
	{
		MouseStateChange stateChange{};
		stateChange.Position = newPosition;
		stateChange.MoveDelta = delta;
		return stateChange;
	}

	MouseStateChange MouseStateChange::ScrollStateChange(const Vector2Int& scrollDelta)
	{
		MouseStateChange stateChange{};
		stateChange.ScrollDelta = scrollDelta;
		return stateChange;
	}

	MouseStateChange MouseStateChange::ButtonStateChange(MouseButton button, bool isPressed)
	{
		MouseStateChange stateChange{};
		stateChange.Button = button;
		stateChange.IsButtonPressed = isPressed;
		return stateChange;
	}

	Mouse::Mouse() :
		_preProcessStateChanges(),
		_currentState{},
		_previousState{},
		_isFirstState(true)
	{}

	void Mouse::UpdateButtonState(MouseButton button, bool isPressed)
	{
		bool& currentState = _currentState.ButtonStates.at(static_cast<size_t>(button));

		if (currentState == isPressed)
			return;

		_preProcessStateChanges.push_back(MouseStateChange::ButtonStateChange(button, isPressed));
		currentState = isPressed;
	}

	void Mouse::UpdatePositionState(const Vector2Int& newPosition)
	{
		Vector2Int& currentPos = _currentState.Position;

		if (currentPos.Equals(newPosition))
			return;

		_preProcessStateChanges.push_back(MouseStateChange::MoveStateChange(newPosition, newPosition - currentPos));
		currentPos = newPosition;
	}

	void Mouse::UpdateScrollState(const Vector2Int& scrollDelta)
	{
		Vector2Int& currentDelta = _currentState.ScrollDelta;

		if (currentDelta.Equals(scrollDelta))
			return;

		_preProcessStateChanges.push_back(MouseStateChange::ScrollStateChange(scrollDelta));
		currentDelta = scrollDelta;
	}

	void Mouse::ClearAllButtonStates()
	{
		for (uint8 i = 0; i < MouseState::ButtonCount; i++)
		{
			if (_currentState.ButtonStates.at(i))
				UpdateButtonState(static_cast<MouseButton>(i), false);
		}
	}

	bool Mouse::IsButtonPressed(MouseButton button) const
	{
		return _currentState.ButtonStates.at(static_cast<size_t>(button));
	}

	bool Mouse::WasButtonJustPressed(MouseButton button) const
	{
		const size_t index = static_cast<size_t>(button);

		return _currentState.ButtonStates.at(index) && !_previousState.ButtonStates.at(index);
	}

	bool Mouse::WasButtonJustReleased(MouseButton button) const
	{
		const size_t index = static_cast<size_t>(button);

		return !_currentState.ButtonStates.at(index) && _previousState.ButtonStates.at(index);
	}

	void Mouse::ProcessCurrentState()
	{
		// Set the previous state to be the current on the first frame to prevent large delta spikes
		if (_isFirstState)
		{
			_previousState = _currentState;
			_isFirstState = false;
			return;
		}

		MouseState tempState = _currentState;
		_currentState = _previousState;

		for (const auto& state : _preProcessStateChanges)
		{
			if (state.Button.has_value())
			{
				_currentState.ButtonStates.at(static_cast<size_t>(state.Button.value())) = state.IsButtonPressed;

				if (state.IsButtonPressed)
				{
					OnButtonPressed.Invoke(state.Button.value());
				}
				else
				{
					OnButtonReleased.Invoke(state.Button.value());
				}
			}
			
			if (state.Position.has_value())
			{
				_currentState.Position = state.Position.value();

				OnMoved.Invoke(state.Position.value(), state.MoveDelta);
			}

			if (state.ScrollDelta.has_value())
			{
				_currentState.ScrollDelta = state.ScrollDelta.value();

				OnScrolled.Invoke(state.ScrollDelta.value());
			}
		}

		_currentState = tempState;
		_preProcessStateChanges.clear();
	}

	void Mouse::SavePreviousState()
	{
		_previousState = _currentState;

		// Reset the scroll delta since we don't have an explicit "stopped scrolling" state change
		_currentState.ScrollDelta = Vector2Int::Zero;
	}
}