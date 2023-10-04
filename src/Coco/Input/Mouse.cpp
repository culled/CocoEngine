#include "Inputpch.h"
#include "Mouse.h"

namespace Coco::Input
{
	MouseState::MouseState() :
		ButtonStates{false},
		Position(Vector2Int::Zero),
		MoveDelta(Vector2Int::Zero),
		ScrollDelta(Vector2Int::Zero)
	{}

	MouseStateChange::MouseStateChange() :
		Button(),
		IsButtonPressed(false),
		Position(),
		MoveDelta(),
		ScrollDelta()
	{}

	MouseStateChange MouseStateChange::PositionStateChange(const Vector2Int& newPosition)
	{
		MouseStateChange stateChange{};
		stateChange.Position = newPosition;
		return stateChange;
	}

	MouseStateChange MouseStateChange::MoveDeltaStateChange(const Vector2Int& delta)
	{
		MouseStateChange stateChange{};
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

		try
		{
			if (isPressed)
				OnButtonPressed.Invoke(button);
			else
				OnButtonReleased.Invoke(button);
		}
		catch(...)
		{}
	}

	void Mouse::UpdatePositionState(const Vector2Int& newPosition)
	{
		Vector2Int& currentPos = _currentState.Position;

		if (currentPos.Equals(newPosition))
			return;

		_preProcessStateChanges.push_back(MouseStateChange::PositionStateChange(newPosition));
		currentPos = newPosition;

		try
		{
			OnPositionChanged.Invoke(newPosition);
		}
		catch (...)
		{}
	}

	void Mouse::UpdateMoveDeltaState(const Vector2Int& moveDelta)
	{
		_preProcessStateChanges.push_back(MouseStateChange::MoveDeltaStateChange(moveDelta));
		_currentState.MoveDelta = moveDelta;

		try
		{
			OnMoved.Invoke(moveDelta);
		}
		catch (...)
		{}
	}

	void Mouse::UpdateScrollState(const Vector2Int& scrollDelta)
	{
		Vector2Int& currentDelta = _currentState.ScrollDelta;

		if (currentDelta.Equals(scrollDelta))
			return;

		_preProcessStateChanges.push_back(MouseStateChange::ScrollStateChange(scrollDelta));
		currentDelta = scrollDelta;

		try
		{
			OnScrolled.Invoke(scrollDelta);
		}
		catch (...)
		{}
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

	void Mouse::SavePreviousState()
	{
		_previousState = _currentState;

		// Reset deltas since we don't have an explicit "stopped" state change
		_currentState.MoveDelta = Vector2Int::Zero;
		_currentState.ScrollDelta = Vector2Int::Zero;

		_preProcessStateChanges.clear();
	}
}