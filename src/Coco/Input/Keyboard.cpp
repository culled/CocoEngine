#include "Inputpch.h"
#include "Keyboard.h"

namespace Coco::Input
{
	KeyboardState::KeyboardState() :
		KeyStates{false},
		UnicodeKeyPoints()
	{}

	KeyboardStateChange::KeyboardStateChange(KeyboardKey key, bool isPressed) :
		Key(key),
		IsPressed(isPressed),
		UnicodeKeyPoint()
	{}

	KeyboardStateChange::KeyboardStateChange(int unicodeKeyPoint) :
		Key(),
		IsPressed(false),
		UnicodeKeyPoint(unicodeKeyPoint)
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

		try
		{
			if (isPressed)
				OnKeyPressed.Invoke(key);
			else
				OnKeyReleased.Invoke(key);
		}
		catch(...)
		{ }
	}

	void Keyboard::AddUnicodeKeyEvent(int unicodeCharacter)
	{
		_preProcessStateChanges.emplace_back(unicodeCharacter);
		_currentState.UnicodeKeyPoints.push_back(unicodeCharacter);

		try
		{
			OnUnicodeCharacterEntered.Invoke(unicodeCharacter);
		}
		catch (...)
		{ }
	}

	void Keyboard::ClearAllKeyStates()
	{
		for (uint32 i = 0; i < KeyboardState::KeyCount; i++)
		{
			if (_currentState.KeyStates.at(i))
				UpdateKeyState(static_cast<KeyboardKey>(i), false);
		}
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

	void Keyboard::SavePreviousState()
	{
		_previousState = _currentState;
		_currentState.UnicodeKeyPoints.clear();

		_preProcessStateChanges.clear();
	}
}