#include "Mouse.h"

namespace Coco::Input
{
	void Mouse::UpdateButtonState(Button button, bool isPressed)
	{
		int index = static_cast<int>(button);
		if (_currentState.ButtonStates[index] == isPressed)
			return;

		_currentState.ButtonStates[index] = isPressed;

		if (isPressed)
		{
			OnButtonPressed.InvokeEvent(button);
		}
		else
		{
			OnButtonReleased.InvokeEvent(button);
		}
	}

	void Mouse::UpdatePositionState(const Vector2Int& newPosition)
	{
		if (_currentState.Position == newPosition)
			return;

		Vector2Int previousPosition = _currentState.Position;

		_currentState.Position = newPosition;

		Vector2Int delta = newPosition - previousPosition;
		OnMoved.InvokeEvent(newPosition, delta);
	}

	void Mouse::UpdateScrollState(const Vector2Int& scrollDelta)
	{
		if (scrollDelta == Vector2Int::Zero)
			return;

		OnScrolled.InvokeEvent(scrollDelta);
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

	void Mouse::SavePreviousState()
	{
		_previousState = _currentState;
	}
}