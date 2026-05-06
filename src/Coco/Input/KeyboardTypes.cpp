//
// Created by cullen on 3/1/26.
//
#include "KeyboardTypes.h"

namespace Coco
{
    const char* ToString(KeyboardKey key)
    {
        switch (key)
        {
            case KeyboardKey::Backspace: return "Backspace";
            case KeyboardKey::Tab: return "Tab";
            case KeyboardKey::Enter: return "Enter";
            case KeyboardKey::Pause: return "Pause";
            case KeyboardKey::CapsLock: return "CapsLock";
            case KeyboardKey::Escape: return "Escape";
            case KeyboardKey::Space: return "Space";
            case KeyboardKey::PageUp: return "PageUp";
            case KeyboardKey::PageDown: return "PageDown";
            case KeyboardKey::End: return "End";
            case KeyboardKey::Home: return "Home";
            case KeyboardKey::ArrowLeft: return "ArrowLeft";
            case KeyboardKey::ArrowUp: return "ArrowUp";
            case KeyboardKey::ArrowRight: return "ArrowRight";
            case KeyboardKey::ArrowDown: return "ArrowDown";
            case KeyboardKey::Print: return "Print";
            case KeyboardKey::Execute: return "Execute";
            case KeyboardKey::PrintScreen: return "PrintScreen";
            case KeyboardKey::Insert: return "Insert";
            case KeyboardKey::Delete: return "Delete";
            case KeyboardKey::D0: return "D0";
            case KeyboardKey::D1: return "D1";
            case KeyboardKey::D2: return "D2";
            case KeyboardKey::D3: return "D3";
            case KeyboardKey::D4: return "D4";
            case KeyboardKey::D5: return "D5";
            case KeyboardKey::D6: return "D6";
            case KeyboardKey::D7: return "D7";
            case KeyboardKey::D8: return "D8";
            case KeyboardKey::D9: return "D9";
            case KeyboardKey::A: return "A";
            case KeyboardKey::B: return "B";
            case KeyboardKey::C: return "C";
            case KeyboardKey::D: return "D";
            case KeyboardKey::E: return "E";
            case KeyboardKey::F: return "F";
            case KeyboardKey::G: return "G";
            case KeyboardKey::H: return "H";
            case KeyboardKey::I: return "I";
            case KeyboardKey::J: return "J";
            case KeyboardKey::K: return "K";
            case KeyboardKey::L: return "L";
            case KeyboardKey::M: return "M";
            case KeyboardKey::N: return "N";
            case KeyboardKey::O: return "O";
            case KeyboardKey::P: return "P";
            case KeyboardKey::Q: return "Q";
            case KeyboardKey::R: return "R";
            case KeyboardKey::S: return "S";
            case KeyboardKey::T: return "T";
            case KeyboardKey::U: return "U";
            case KeyboardKey::V: return "V";
            case KeyboardKey::W: return "W";
            case KeyboardKey::X: return "X";
            case KeyboardKey::Y: return "Y";
            case KeyboardKey::Z: return "Z";
            case KeyboardKey::LeftMeta: return "LeftMeta";
            case KeyboardKey::RightMeta: return "RightMeta";
            case KeyboardKey::Numpad0: return "Numpad0";
            case KeyboardKey::Numpad1: return "Numpad1";
            case KeyboardKey::Numpad2: return "Numpad2";
            case KeyboardKey::Numpad3: return "Numpad3";
            case KeyboardKey::Numpad4: return "Numpad4";
            case KeyboardKey::Numpad5: return "Numpad5";
            case KeyboardKey::Numpad6: return "Numpad6";
            case KeyboardKey::Numpad7: return "Numpad7";
            case KeyboardKey::Numpad8: return "Numpad8";
            case KeyboardKey::Numpad9: return "Numpad9";
            case KeyboardKey::NumpadMultiply: return "NumpadMultiply";
            case KeyboardKey::NumpadPlus: return "NumpadPlus";
            case KeyboardKey::Separator: return "Separator";
            case KeyboardKey::NumpadMinus: return "NumpadMinus";
            case KeyboardKey::NumpadDecimal: return "NumpadDecimal";
            case KeyboardKey::NumpadDivide: return "NumpadDivide";
            case KeyboardKey::F1: return "F1";
            case KeyboardKey::F2: return "F2";
            case KeyboardKey::F3: return "F3";
            case KeyboardKey::F4: return "F4";
            case KeyboardKey::F5: return "F5";
            case KeyboardKey::F6: return "F6";
            case KeyboardKey::F7: return "F7";
            case KeyboardKey::F8: return "F8";
            case KeyboardKey::F9: return "F9";
            case KeyboardKey::F10: return "F10";
            case KeyboardKey::F11: return "F11";
            case KeyboardKey::F12: return "F12";
            case KeyboardKey::F13: return "F13";
            case KeyboardKey::F14: return "F14";
            case KeyboardKey::F15: return "F15";
            case KeyboardKey::F16: return "F16";
            case KeyboardKey::F17: return "F17";
            case KeyboardKey::F18: return "F18";
            case KeyboardKey::F19: return "F19";
            case KeyboardKey::F20: return "F20";
            case KeyboardKey::F21: return "F21";
            case KeyboardKey::F22: return "F22";
            case KeyboardKey::F23: return "F23";
            case KeyboardKey::F24: return "F24";
            case KeyboardKey::NumLock: return "NumLock";
            case KeyboardKey::ScrollLock: return "ScrollLock";
            case KeyboardKey::LeftShift: return "LeftShift";
            case KeyboardKey::RightShift: return "RightShift";
            case KeyboardKey::LeftControl: return "LeftControl";
            case KeyboardKey::RightControl: return "RightControl";
            case KeyboardKey::LeftAlt: return "LeftAlt";
            case KeyboardKey::RightAlt: return "RightAlt";
            case KeyboardKey::Semicolon: return "Semicolon";
            case KeyboardKey::Equals: return "Equals";
            case KeyboardKey::Comma: return "Comma";
            case KeyboardKey::Minus: return "Minus";
            case KeyboardKey::Period: return "Period";
            case KeyboardKey::ForwardSlash: return "ForwardSlash";
            case KeyboardKey::BackQuote: return "BackQuote";
            case KeyboardKey::LeftBracket: return "LeftBracket";
            case KeyboardKey::BackSlash: return "BackSlash";
            case KeyboardKey::RightBracket: return "RightBracket";
            case KeyboardKey::SingleQuote: return "SingleQuote";
            default: return "Unknown";
        }
    }

    KeyboardKeyInputEvent::KeyboardKeyInputEvent(KeyboardKey key, bool isPressed,
        KeyboardModifierKeyFlags modifierKeyFlags) noexcept :
        ButtonInputEvent(isPressed),
        Key(key),
        ModifierKeys(modifierKeyFlags)
    {}

    KeyboardUnicodeInputEvent::KeyboardUnicodeInputEvent(uint32 unicodePoint) noexcept :
        UnicodePoint(unicodePoint)
    {}
}
