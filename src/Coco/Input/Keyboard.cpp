#include "Keyboard.h"

namespace Coco::Input
{
	KeyboardStateChange KeyboardStateChange::KeyStateChange(KeyboardKey key, bool isPressed) noexcept
	{
		KeyboardStateChange state = {};
		state.Key = key;
		state.IsPressed = isPressed;
		return state;
	}

	void Keyboard::UpdateKeyState(KeyboardKey key, bool isPressed)
	{
		const int index = static_cast<int>(key);

		if (_preProcessState.KeyState[index] == isPressed)
			return;

		// Save the state change and the current interim state
		_preProcessStateChanges.Add(KeyboardStateChange::KeyStateChange(key, isPressed));
		_preProcessState.KeyState[index] = isPressed;
	}

	bool Keyboard::IsKeyPressed(KeyboardKey key) const noexcept
	{
		return _currentState.KeyState[static_cast<int>(key)];
	}

	bool Keyboard::WasKeyJustPressed(KeyboardKey key) const noexcept
	{
		const int index = static_cast<int>(key);
		return (_currentState.KeyState[index] && !_previousState.KeyState[index]);
	}

	bool Keyboard::WasKeyJustReleased(KeyboardKey key) const noexcept
	{
		const int index = static_cast<int>(key);
		return (!_currentState.KeyState[index] && _previousState.KeyState[index]);
	}

	void Keyboard::ProcessCurrentState()
	{
		// Step through each state change since the last tick and fire the proper events
		for (const auto& newState : _preProcessStateChanges)
		{
			if (newState.Key.has_value())
			{
				if (newState.IsPressed)
				{
					OnKeyPressedEvent.Invoke(newState.Key.value());
				}
				else
				{
					OnKeyReleasedEvent.Invoke(newState.Key.value());
				}
			}
		}

		_currentState = _preProcessState;
		_preProcessStateChanges.Clear();
	}

	void Keyboard::SavePreviousState() noexcept
	{
		_previousState = _currentState;
	}
}