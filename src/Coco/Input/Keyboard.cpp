#include "Inputpch.h"
#include "Keyboard.h"

namespace Coco::Input
{
	KeyboardState::KeyboardState() :
		KeyStates{false}
	{}

	KeyboardStateChange::KeyboardStateChange(KeyboardKey key, bool isPressed) :
		Key(key), IsPressed(isPressed)
	{}

	Keyboard::Keyboard() :
		_preProcessStateChanges{},
		_currentState{},
		_previousState{}
	{}

	void Keyboard::UpdateKeyState(KeyboardKey key, bool isPressed)
	{
		bool& currentState = _currentState.KeyStates.at(static_cast<size_t>(key));

		if (currentState == isPressed)
			return;

		_preProcessStateChanges.emplace_back(key, isPressed);
		currentState = isPressed;
	}

	bool Keyboard::IsKeyPressed(KeyboardKey key) const
	{
		return _currentState.KeyStates.at(static_cast<size_t>(key));
	}

	bool Keyboard::WasKeyJustPressed(KeyboardKey key) const
	{
		const size_t index = static_cast<size_t>(key);

		return _currentState.KeyStates.at(index) && !_previousState.KeyStates.at(index);
	}

	bool Keyboard::WasKeyJustReleased(KeyboardKey key) const
	{
		const size_t index = static_cast<size_t>(key);

		return !_currentState.KeyStates.at(index) && _previousState.KeyStates.at(index);
	}

	void Keyboard::ProcessCurrentState()
	{
		KeyboardState tempState = _currentState;
		_currentState = _previousState;

		// Replay the states and fire their proper events
		for (const auto& state : _preProcessStateChanges)
		{
			_currentState.KeyStates.at(static_cast<size_t>(state.Key)) = state.IsPressed;

			if (state.IsPressed)
			{
				OnKeyPressed.Invoke(state.Key);
			}
			else
			{
				OnKeyReleased.Invoke(state.Key);
			}
		}

		_currentState = tempState;
		_preProcessStateChanges.clear();
	}

	void Keyboard::SavePreviousState()
	{
		_previousState = _currentState;
	}
}