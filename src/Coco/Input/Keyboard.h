#pragma once

#include "Inputpch.h"
#include "InputTypes.h"
#include <Coco/Core/Events/Event.h>

namespace Coco::Input
{
	/// @brief A state for a keyboard
	struct KeyboardState
	{
		/// @brief The maximum number of keys
		static constexpr uint8 KeyCount = 255;

		/// @brief Pressed states for each keyboard key
		std::array<bool, KeyCount> KeyStates;

		/// @brief The unicode characters that were entered since the last tick
		std::vector<int> UnicodeKeyPoints;

		KeyboardState();
	};

	/// @brief A state change for the keyboard
	struct KeyboardStateChange
	{
		/// @brief The key that changed state, if any
		std::optional<KeyboardKey> Key;

		/// @brief If true, the key is now pressed, else it is released
		bool IsPressed;

		/// @brief Represents the unicode character point that was just entered
		std::optional<int> UnicodeKeyPoint;

		KeyboardStateChange(KeyboardKey key, bool isPressed);
		KeyboardStateChange(int unicodeKeyPoint);
	};

	/// @brief A keyboard input device
	class Keyboard
	{
		friend class InputService;

	public:
		/// @brief Invoked when a key is pressed
		Event<KeyboardKey> OnKeyPressed;

		/// @brief Invoked when a key is released
		Event<KeyboardKey> OnKeyReleased;

		/// @brief Invoked when a unicode character is input
		Event<int> OnUnicodeCharacterEntered;

	private:
		std::vector<KeyboardStateChange> _preProcessStateChanges;
		KeyboardState _currentState;
		KeyboardState _previousState;

	public:
		Keyboard();
		~Keyboard() = default;

		/// @brief Updates the pressed state for a key
		/// @param key The key
		/// @param isPressed True if the key is pressed
		void UpdateKeyState(KeyboardKey key, bool isPressed);
		
		/// @brief Adds a unicode character event
		/// @param unicodeCharacter The character code
		void AddUnicodeKeyEvent(int unicodeCharacter);

		/// @brief Clears all current key states
		void ClearAllKeyStates();

		/// @brief Determines if the given key is currently pressed
		/// @param key The key
		/// @return True if the key is currently pressed
		bool IsKeyPressed(KeyboardKey key) const;

		/// @brief Determines if the given key was just pressed (unpressed -> pressed) within the last tick
		/// @param key The key
		/// @return True if the key was pressed since the last tick
		bool WasKeyJustPressed(KeyboardKey key) const;

		/// @brief Determines if the given key was just released (pressed -> unpressed) within the last tick
		/// @param key The key
		/// @return True if the key was released since the last tick
		bool WasKeyJustReleased(KeyboardKey key) const;

		/// @brief Gets the unicode characters that were entered within the last tick
		/// @return The unicode characters
		std::span<const int> GetUnicodeCharactersEntered() const { return _currentState.UnicodeKeyPoints; }

	private:
		/// @brief Gets all state changes since the last tick
		/// @return The state changes
		std::span<const KeyboardStateChange> GetStateChanges() const { return _preProcessStateChanges; }

		/// @brief Saves the current state as the previous state
		void SavePreviousState();
	};
}

