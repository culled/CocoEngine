#pragma once

namespace Coco::Input
{
	/// @brief Mouse buttons
	enum class MouseButton
	{
		Left,
		Middle,
		Right,
		Button3,
		Button4,
		Unknown,
	};

    /// @brief Keyboard keys
    enum class KeyboardKey
    {
        Backspace = 0x08,
        Tab = 0x09,

        Enter = 0x0D,
        Return = Enter,

        Pause = 0x13,
        CapsLock = 0x14,

        Escape = 0x1B,

        Space = 0x20,
        PageUp = 0x21,
        PageDown = 0x22,
        End = 0x23,
        Home = 0x24,
        ArrowLeft = 0x25,
        ArrowUp = 0x26,
        ArrowRight = 0x27,
        ArrowDown = 0x28,

        Print = 0x2A,
        Execute = 0x2B,
        PrintScreen = 0x2C,
        Insert = 0x2D,
        Delete = 0x2E,

        D0 = 0x30,
        D1 = 0x31,
        D2 = 0x32,
        D3 = 0x33,
        D4 = 0x34,
        D5 = 0x35,
        D6 = 0x36,
        D7 = 0x37,
        D8 = 0x38,
        D9 = 0x39,

        A = 0x41,
        B = 0x42,
        C = 0x43,
        D = 0x44,
        E = 0x45,
        F = 0x46,
        G = 0x47,
        H = 0x48,
        I = 0x49,
        J = 0x4A,
        K = 0x4B,
        L = 0x4C,
        M = 0x4D,
        N = 0x4E,
        O = 0x4F,
        P = 0x50,
        Q = 0x51,
        R = 0x52,
        S = 0x53,
        T = 0x54,
        U = 0x55,
        V = 0x56,
        W = 0x57,
        X = 0x58,
        Y = 0x59,
        Z = 0x5A,

        LeftMeta = 0x5B,
        RightMeta = 0x5C,

        Numpad0 = 0x60,
        Numpad1 = 0x61,
        Numpad2 = 0x62,
        Numpad3 = 0x63,
        Numpad4 = 0x64,
        Numpad5 = 0x65,
        Numpad6 = 0x66,
        Numpad7 = 0x67,
        Numpad8 = 0x68,
        Numpad9 = 0x69,

        NumpadMultiply = 0x6A,
        NumpadPlus = 0x6B,
        Separator = 0x6C,
        NumpadMinus = 0x6D,
        NumpadDecimal = 0x6E,
        NumpadDivide = 0x6F,

        F1 = 0x70,
        F2 = 0x71,
        F3 = 0x72,
        F4 = 0x73,
        F5 = 0x74,
        F6 = 0x75,
        F7 = 0x76,
        F8 = 0x77,
        F9 = 0x78,
        F10 = 0x79,
        F11 = 0x7A,
        F12 = 0x7B,
        F13 = 0x7C,
        F14 = 0x7D,
        F15 = 0x7E,
        F16 = 0x7F,
        F17 = 0x80,
        F18 = 0x81,
        F19 = 0x82,
        F20 = 0x83,
        F21 = 0x84,
        F22 = 0x85,
        F23 = 0x86,
        F24 = 0x87,

        NumLock = 0x90,
        ScrollLock = 0x91,

        LeftShift = 0xA0,
        RightShift = 0xA1,
        LeftControl = 0xA2,
        RightControl = 0xA3,
        LeftAlt = 0xA4,
        RightAlt = 0xA5,

        Oem1 = 0xBA,
        Semicolon = Oem1,

        Equals = 0xBB,
        Comma = 0xBC,
        Minus = 0xBD,
        Period = 0xBE,

        Oem2 = 0xBF,
        ForwardSlash = Oem2,

        Oem3 = 0xC0,
        BackQuote = Oem3,

        Oem4 = 0xDB,
        LeftBracket = Oem4,

        Oem5 = 0xDC,
        BackSlash = Oem5,

        Oem6 = 0xDD,
        RightBracket = Oem6,

        Oem7 = 0xDF,
        SingleQuote = Oem7,

        Unknown,
    };
}