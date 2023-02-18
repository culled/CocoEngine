#include "Keyboard.h"

namespace Coco::Input
{
	KeyboardStateChange KeyboardStateChange::KeyStateChange(int keyIndex, bool isPressed)
	{
		KeyboardStateChange state = {};
		state.KeyIndex = keyIndex;
		state.IsPressed = isPressed;
		return state;
	}

	void Keyboard::UpdateKeyState(Key key, bool isPressed)
	{
		int index = static_cast<int>(key);

		if (_preProcessState.KeyState[index] == isPressed)
			return;

		_preProcessStateChanges.Add(KeyboardStateChange::KeyStateChange(index, isPressed));
		_preProcessState.KeyState[index] = isPressed;
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

	void Keyboard::ProcessCurrentState()
	{
		// Step through each state change since the last tick and fire the proper events
		for (const auto& newState : _preProcessStateChanges)
		{
			if (newState.KeyIndex.has_value())
			{
				if (newState.IsPressed)
				{
					OnKeyPressedEvent.InvokeEvent(static_cast<Keyboard::Key>(newState.KeyIndex.value()));
				}
				else
				{
					OnKeyReleasedEvent.InvokeEvent(static_cast<Keyboard::Key>(newState.KeyIndex.value()));
				}
			}
		}

		_currentState = _preProcessState;
		_preProcessStateChanges.Clear();
	}

	void Keyboard::SavePreviousState()
	{
		_previousState = _currentState;
	}
}