#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Core/Types/Array.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Optional.h>
#include <Coco/Core/Events/Event.h>

#include "InputTypes.h"

namespace Coco::Input
{
    /// @brief A state for a keyboard
    struct KeyboardState
    {
        /// @brief The maximum number of keys
        static constexpr int KeyCount = 256;

        /// @brief Key pressed states
        Array<bool, KeyCount> KeyState = {false};
    };

    /// @brief A state change for the keyboard
    struct KeyboardStateChange
    {
        /// @brief The key that changed
        Optional<KeyboardKey> Key;

        /// @brief If true, the key was pressed, else it was released
        bool IsPressed = false;

        /// @brief Creates a keyboard state change for a key that was pressed or released
        /// @param key The key
        /// @param isPressed True if the key was pressed, false if it was released
        /// @return A state change
        static KeyboardStateChange KeyStateChange(KeyboardKey key, bool isPressed) noexcept;
    };

	/// @brief A keyboard input device
	class COCOAPI Keyboard
	{
	public:
        /// @brief Invoked when a key is pressed
        Event<KeyboardKey> OnKeyPressedEvent;

        /// @brief Invoked when a key is released
        Event<KeyboardKey> OnKeyReleasedEvent;

    private:
        friend class InputService;

        List<KeyboardStateChange> _preProcessStateChanges;
        KeyboardState _preProcessState = {};
        KeyboardState _currentState = {};
        KeyboardState _previousState = {};

    public:
        /// @brief Updates the pressed state for a key
        /// @param key The key
        /// @param isPressed True if the key is pressed
        void UpdateKeyState(KeyboardKey key, bool isPressed);

        /// @brief Gets if the given key is currently pressed
        /// @param key The key
        /// @return True if the key is currently pressed
        bool IsKeyPressed(KeyboardKey key) const noexcept;

        /// @brief Gets if the given key was just pressed (unpressed -> pressed) within the last tick
        /// @param key The key
        /// @return True if the key was pressed since the last tick
        bool WasKeyJustPressed(KeyboardKey key) const noexcept;

        /// @brief Gets if the given key was just released (pressed -> unpressed) within the last tick
        /// @param key The key
        /// @return True if the key was released since the last tick
        bool WasKeyJustReleased(KeyboardKey key) const noexcept;

    private:
        /// @brief Processes all state changes since the last tick
        void ProcessCurrentState();

        /// @brief Saves the current state as the previous state
        void SavePreviousState() noexcept;
	};
}
