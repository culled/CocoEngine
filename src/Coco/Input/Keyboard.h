//
// Created by cullen on 3/1/26.
//

#ifndef COCOENGINE_KEYBOARD_H
#define COCOENGINE_KEYBOARD_H
#include <Coco/Core/Types/CoreTypes.h>
#include <Coco/Core/Types/Optional.h>
#include "KeyboardTypes.h"

namespace Coco
{
    /// @brief A state for a keyboard
    struct KeyboardState
    {
        /// @brief The maximum number of keys
        static constexpr uint8 KeyCount = 225;

        /// @brief Pressed states for each keyboard key
        bool KeyStates[KeyCount];

        /// @brief The last unicode character that was entered
        Optional<uint32> UnicodeCharacter;

        KeyboardState();
        KeyboardState& operator=(const KeyboardState& rhs);
    };

    class InputService;

    class Keyboard
    {
        friend class InputService;

    public:
        Keyboard(InputService* inputService);

        /// @brief Adds a state change event for a key
        /// @param key The key
        /// @param isPressed The pressed state of the key
        void AddKeyStateChangeEvent(KeyboardKey key, bool isPressed);

        /// @brief Adds an event for a unicode character that was entered
        /// @param unicodeCharacter The character that was entered
        void AddUnicodeCharacterEvent(uint32 unicodeCharacter);

        /// @brief Determines if the given key was pressed in the last tick
        /// @param key The key
        /// @return True if the key transitioned from released->pressed in the last tick
        bool WasKeyJustPressed(KeyboardKey key) const;

        /// @brief Determines if the given key was released in the last tick
        /// @param key The key
        /// @return True if the key transitioned from pressed->released in the last tick
        bool WasKeyJustReleased(KeyboardKey key) const;

        /// @brief Determines if the given key is currently pressed
        /// @param key The key
        /// @return True if the key is currently pressed
        bool IsKeyPressed(KeyboardKey key) const;

        /// @brief Gets the currently pressed modifier keys
        /// @return The currently pressed modifier keys
        KeyboardModifierKeyFlags GetCurrentlyPressedModifierKeys() const;

        /// @brief Determines if the given modifier keys are currently pressed
        /// @param modifierKeys The modifier keys
        /// @return True if the modifier keys are pressed
        bool AreModifierKeysPressed(KeyboardModifierKeyFlags modifierKeys) const;

        /// @brief Determines if a unicode character was entered in the last tick
        /// @return True if a unicode character was entered in the last tick
        bool HasUnicodeCharacter() const { return _currentState.UnicodeCharacter.has_value(); }

        /// @brief Gets the unicode that was just entered
        /// @return The unicode character that was just entered
        uint32 GetUnicodeCharacter() const { return _currentState.UnicodeCharacter ? _currentState.UnicodeCharacter.value() : 0; }

    private:
        InputService* _inputService;
        KeyboardState _currentState;
        KeyboardState _previousState;

    private:
        /// @brief Saves the previous input state
        void SavePreviousState();

        /// @brief Resets the state of all Mouse buttons
        void ResetState();
    };
} // Coco

#endif //COCOENGINE_KEYBOARD_H