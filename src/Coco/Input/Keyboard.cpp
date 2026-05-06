//
// Created by cullen on 3/1/26.
//

#include "Keyboard.h"

#include <cstring>
#include <Coco/Core/Asserts.h>

#include "InputService.h"

namespace Coco
{
    KeyboardState::KeyboardState() :
        KeyStates{false},
        UnicodeCharacter()
    {}

    KeyboardState& KeyboardState::operator=(const KeyboardState& rhs)
    {
        memcpy(KeyStates, rhs.KeyStates, sizeof(KeyStates));
        UnicodeCharacter = rhs.UnicodeCharacter;
        return *this;
    }

    Keyboard::Keyboard(InputService* inputService) :
        _inputService(inputService),
        _currentState(),
        _previousState()
    {}

    void Keyboard::AddKeyStateChangeEvent(KeyboardKey key, bool isPressed)
    {
        COCO_ASSERT(key <= KeyboardKey::Unknown, "Invalid keyboard key");
        int keyIndex = static_cast<int>(key);

        if (_currentState.KeyStates[keyIndex] != isPressed)
        {
            _currentState.KeyStates[keyIndex] = isPressed;
            _inputService->DispatchInputEvent(KeyboardKeyInputEvent(key, isPressed, GetCurrentlyPressedModifierKeys()));
        }
    }

    void Keyboard::AddUnicodeCharacterEvent(uint32 unicodeCharacter)
    {
        _currentState.UnicodeCharacter = unicodeCharacter;
        _inputService->DispatchInputEvent(KeyboardUnicodeInputEvent(unicodeCharacter));
    }

    bool Keyboard::WasKeyJustPressed(KeyboardKey key) const
    {
        COCO_ASSERT(key <= KeyboardKey::Unknown, "Invalid keyboard key");
        int keyIndex = static_cast<int>(key);

        return _currentState.KeyStates[keyIndex] && !_previousState.KeyStates[keyIndex];
    }

    bool Keyboard::WasKeyJustReleased(KeyboardKey key) const
    {
        COCO_ASSERT(key <= KeyboardKey::Unknown, "Invalid keyboard key");
        int keyIndex = static_cast<int>(key);

        return _previousState.KeyStates[keyIndex] && !_currentState.KeyStates[keyIndex];
    }

    bool Keyboard::IsKeyPressed(KeyboardKey key) const
    {
        COCO_ASSERT(key <= KeyboardKey::Unknown, "Invalid keyboard key");
        int keyIndex = static_cast<int>(key);

        return _currentState.KeyStates[keyIndex];
    }

    KeyboardModifierKeyFlags Keyboard::GetCurrentlyPressedModifierKeys() const
    {
        KeyboardModifierKeyFlags result = KeyboardModifierKeyFlags::None;

        if (IsKeyPressed(KeyboardKey::LeftControl) || IsKeyPressed(KeyboardKey::RightControl))
            result |= KeyboardModifierKeyFlags::Control;

        if (IsKeyPressed(KeyboardKey::LeftShift) || IsKeyPressed(KeyboardKey::RightShift))
            result |= KeyboardModifierKeyFlags::Shift;

        if (IsKeyPressed(KeyboardKey::LeftAlt) || IsKeyPressed(KeyboardKey::RightAlt))
            result |= KeyboardModifierKeyFlags::Alt;

        if (IsKeyPressed(KeyboardKey::LeftMeta) || IsKeyPressed(KeyboardKey::RightMeta))
            result |= KeyboardModifierKeyFlags::Meta;

        return result;
    }

    bool Keyboard::AreModifierKeysPressed(KeyboardModifierKeyFlags modifierKeys) const
    {
        return (GetCurrentlyPressedModifierKeys() & modifierKeys) == modifierKeys;
    }

    void Keyboard::SavePreviousState()
    {
        _previousState = _currentState;
        _currentState.UnicodeCharacter.reset();
    }

    void Keyboard::ResetState()
    {
        for (uint8 i = 0; i < KeyboardState::KeyCount; ++i)
        {
            if (_currentState.KeyStates[i])
                AddKeyStateChangeEvent(static_cast<KeyboardKey>(i), false);
        }
    }
} // Coco