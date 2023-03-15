#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/Array.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Optional.h>
#include <Coco/Core/Events/Event.h>

#include "InputTypes.h"

namespace Coco::Input
{
    /// <summary>
    /// A state for a keyboard
    /// </summary>
    struct KeyboardState
    {
        /// <summary>
        /// The maximum number of keys
        /// </summary>
        static constexpr int KeyCount = 256;

        /// <summary>
        /// Key pressed states
        /// </summary>
        Array<bool, KeyCount> KeyState = {false};
    };

    /// <summary>
    /// A state change for the keyboard
    /// </summary>
    struct KeyboardStateChange
    {
        /// <summary>
        /// The key that changed
        /// </summary>
        Optional<KeyboardKey> Key;

        /// <summary>
        /// If true, the key was pressed, else it was released
        /// </summary>
        bool IsPressed = false;

        /// <summary>
        /// Creates a keyboard state change for a key that was pressed or released
        /// </summary>
        /// <param name="key">The key</param>
        /// <param name="isPressed">True if the key was pressed, false if it was released</param>
        /// <returns>A state change</returns>
        static constexpr KeyboardStateChange KeyStateChange(KeyboardKey key, bool isPressed) noexcept
        {
            KeyboardStateChange state = {};
            state.Key = key;
            state.IsPressed = isPressed;
            return state;
        }
    };

	/// <summary>
	/// Represents a keyboard
	/// </summary>
	class COCOAPI Keyboard
	{
	public:
        /// <summary>
        /// Invoked when a key is pressed
        /// </summary>
        Event<KeyboardKey> OnKeyPressedEvent;

        /// <summary>
        /// Invoked when a key is released
        /// </summary>
        Event<KeyboardKey> OnKeyReleasedEvent;

    private:
        friend class InputService;

        List<KeyboardStateChange> _preProcessStateChanges;
        KeyboardState _preProcessState = {};
        KeyboardState _currentState = {};
        KeyboardState _previousState = {};

    public:
        /// <summary>
        /// Updates the pressed state for a key
        /// </summary>
        /// <param name="key">The key</param>
        /// <param name="isPressed">True if the key is pressed</param>
        void UpdateKeyState(KeyboardKey key, bool isPressed);

        /// <summary>
        /// Gets if the given key is currently pressed
        /// </summary>
        /// <param name="key">The key</param>
        /// <returns>True if the key is currently pressed</returns>
        bool IsKeyPressed(KeyboardKey key) const noexcept;

        /// <summary>
        /// Gets if the given key was just pressed (unpressed -> pressed) within the last tick
        /// </summary>
        /// <param name="key">The key</param>
        /// <returns>True if the key was pressed since the last tick</returns>
        bool WasKeyJustPressed(KeyboardKey key) const noexcept;

        /// <summary>
        /// Gets if the given key was just released (pressed -> unpressed) within the last tick
        /// </summary>
        /// <param name="key">The key</param>
        /// <returns>True if the key was released since the last tick</returns>
        bool WasKeyJustReleased(KeyboardKey key) const noexcept;

    private:
        /// <summary>
        /// Processes all state changes since the last tick
        /// </summary>
        void ProcessCurrentState();

        /// <summary>
        /// Saves the current state as the previous state
        /// </summary>
        void SavePreviousState() noexcept;
	};
}
