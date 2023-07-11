#include "Keyboard.h"

namespace Coco::Input
{
	void Keyboard::UpdateKeyState(KeyboardKey key, bool isPressed)
	{
		bool& state = _preProcessState.KeyState.at(static_cast<int>(key));

		if (state == isPressed)
			return;

		// Save the state change and the current interim state
		_preProcessStateChanges.Construct(key, isPressed);
		state = isPressed;
	}

	bool Keyboard::IsKeyPressed(KeyboardKey key) const noexcept
	{
		return _currentState.KeyState.at(static_cast<int>(key));
	}

	bool Keyboard::WasKeyJustPressed(KeyboardKey key) const noexcept
	{
		const int index = static_cast<int>(key);
		return (_currentState.KeyState.at(index) && !_previousState.KeyState.at(index));
	}

	bool Keyboard::WasKeyJustReleased(KeyboardKey key) const noexcept
	{
		const int index = static_cast<int>(key);
		return (!_currentState.KeyState.at(index) && _previousState.KeyState.at(index));
	}

	void Keyboard::ProcessCurrentState()
	{
		// Step through each state change since the last tick and fire the proper events
		for (const auto& newState : _preProcessStateChanges)
		{
			if (newState.IsPressed)
			{
				OnKeyPressedEvent.Invoke(newState.Key);
			}
			else
			{
				OnKeyReleasedEvent.Invoke(newState.Key);
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