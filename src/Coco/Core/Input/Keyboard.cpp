#include "Keyboard.h"

namespace Coco::Input
{
	void Keyboard::UpdateKeyState(Key key, bool isPressed)
	{
		int index = static_cast<int>(key);

		if (_currentState.KeyState[index] == isPressed)
			return;

		_currentState.KeyState[index] = isPressed;

		if (isPressed)
		{
			OnKeyPressedEvent.InvokeEvent(key);
		}
		else
		{
			OnKeyReleasedEvent.InvokeEvent(key);
		}
	}

	bool Keyboard::IsKeyPressed(Key key) const
	{
		return _currentState.KeyState[static_cast<int>(key)];
	}

	bool Keyboard::WasKeyJustPressed(Key key) const
	{
		int index = static_cast<int>(key);
		return (_currentState.KeyState[index] && !_previousState.KeyState[index]);
	}

	bool Keyboard::WasKeyJustReleased(Key key) const
	{
		int index = static_cast<int>(key);
		return (!_currentState.KeyState[index] && _previousState.KeyState[index]);
	}

	void Keyboard::SavePreviousState()
	{
		_previousState = _currentState;
	}
}