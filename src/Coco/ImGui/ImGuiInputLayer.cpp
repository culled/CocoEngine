#include "ImGuiInputLayer.h"

#include <Coco/Input/InputService.h>

#include "ImGuiCocoPlatform.h"
#include <Coco/Core/Engine.h>

#include <imgui.h>

namespace Coco::ImGuiCoco
{
	const int ImGuiInputLayer::Priority = -10;

    ImGuiKey ImGuiInputLayer::ToImGuiKey(KeyboardKey key)
    {
        switch (key)
        {
        case KeyboardKey::Backspace:
            return ImGuiKey_Backspace;
        case KeyboardKey::Tab:
            return ImGuiKey_Tab;

        case KeyboardKey::Enter:
            return ImGuiKey_Enter;

        case KeyboardKey::Pause:
            return ImGuiKey_Pause;
        case KeyboardKey::CapsLock:
            return ImGuiKey_CapsLock;

        case KeyboardKey::Escape:
            return ImGuiKey_Escape;

        case KeyboardKey::Space:
            return ImGuiKey_Space;
        case KeyboardKey::PageUp:
            return ImGuiKey_PageUp;
        case KeyboardKey::PageDown:
            return ImGuiKey_PageDown;
        case KeyboardKey::End:
            return ImGuiKey_End;
        case KeyboardKey::Home:
            return ImGuiKey_Home;
        case KeyboardKey::ArrowLeft:
            return ImGuiKey_LeftArrow;
        case KeyboardKey::ArrowUp:
            return ImGuiKey_UpArrow;
        case KeyboardKey::ArrowRight:
            return ImGuiKey_RightArrow;
        case KeyboardKey::ArrowDown:
            return ImGuiKey_DownArrow;

        case KeyboardKey::PrintScreen:
            return ImGuiKey_PrintScreen;
        case KeyboardKey::Insert:
            return ImGuiKey_Insert;
        case KeyboardKey::Delete:
            return ImGuiKey_Delete;

        case KeyboardKey::D0:
            return ImGuiKey_0;
        case KeyboardKey::D1:
            return ImGuiKey_1;
        case KeyboardKey::D2:
            return ImGuiKey_2;
        case KeyboardKey::D3:
            return ImGuiKey_3;
        case KeyboardKey::D4:
            return ImGuiKey_4;
        case KeyboardKey::D5:
            return ImGuiKey_5;
        case KeyboardKey::D6:
            return ImGuiKey_6;
        case KeyboardKey::D7:
            return ImGuiKey_7;
        case KeyboardKey::D8:
            return ImGuiKey_8;
        case KeyboardKey::D9:
            return ImGuiKey_9;

        case KeyboardKey::A:
            return ImGuiKey_A;
        case KeyboardKey::B:
            return ImGuiKey_B;
        case KeyboardKey::C:
            return ImGuiKey_C;
        case KeyboardKey::D:
            return ImGuiKey_D;
        case KeyboardKey::E:
            return ImGuiKey_E;
        case KeyboardKey::F:
            return ImGuiKey_F;
        case KeyboardKey::G:
            return ImGuiKey_G;
        case KeyboardKey::H:
            return ImGuiKey_H;
        case KeyboardKey::I:
            return ImGuiKey_I;
        case KeyboardKey::J:
            return ImGuiKey_J;
        case KeyboardKey::K:
            return ImGuiKey_K;
        case KeyboardKey::L:
            return ImGuiKey_L;
        case KeyboardKey::M:
            return ImGuiKey_M;
        case KeyboardKey::N:
            return ImGuiKey_N;
        case KeyboardKey::O:
            return ImGuiKey_O;
        case KeyboardKey::P:
            return ImGuiKey_P;
        case KeyboardKey::Q:
            return ImGuiKey_Q;
        case KeyboardKey::R:
            return ImGuiKey_R;
        case KeyboardKey::S:
            return ImGuiKey_S;
        case KeyboardKey::T:
            return ImGuiKey_T;
        case KeyboardKey::U:
            return ImGuiKey_U;
        case KeyboardKey::V:
            return ImGuiKey_V;
        case KeyboardKey::W:
            return ImGuiKey_W;
        case KeyboardKey::X:
            return ImGuiKey_X;
        case KeyboardKey::Y:
            return ImGuiKey_Y;
        case KeyboardKey::Z:
            return ImGuiKey_Z;

        case KeyboardKey::LeftMeta:
            return ImGuiKey_LeftSuper;
        case KeyboardKey::RightMeta:
            return ImGuiKey_RightSuper;

        case KeyboardKey::Numpad0:
            return ImGuiKey_Keypad0;
        case KeyboardKey::Numpad1:
            return ImGuiKey_Keypad1;
        case KeyboardKey::Numpad2:
            return ImGuiKey_Keypad2;
        case KeyboardKey::Numpad3:
            return ImGuiKey_Keypad3;
        case KeyboardKey::Numpad4:
            return ImGuiKey_Keypad4;
        case KeyboardKey::Numpad5:
            return ImGuiKey_Keypad5;
        case KeyboardKey::Numpad6:
            return ImGuiKey_Keypad6;
        case KeyboardKey::Numpad7:
            return ImGuiKey_Keypad7;
        case KeyboardKey::Numpad8:
            return ImGuiKey_Keypad8;
        case KeyboardKey::Numpad9:
            return ImGuiKey_Keypad9;

        case KeyboardKey::NumpadMultiply:
            return ImGuiKey_KeypadMultiply;
        case KeyboardKey::NumpadPlus:
            return ImGuiKey_KeypadAdd;
        case KeyboardKey::NumpadMinus:
            return ImGuiKey_KeypadSubtract;
        case KeyboardKey::NumpadDecimal:
            return ImGuiKey_KeypadDecimal;
        case KeyboardKey::NumpadDivide:
            return ImGuiKey_KeypadDivide;

        case KeyboardKey::F1:
            return ImGuiKey_F1;
        case KeyboardKey::F2:
            return ImGuiKey_F2;
        case KeyboardKey::F3:
            return ImGuiKey_F3;
        case KeyboardKey::F4:
            return ImGuiKey_F4;
        case KeyboardKey::F5:
            return ImGuiKey_F5;
        case KeyboardKey::F6:
            return ImGuiKey_F6;
        case KeyboardKey::F7:
            return ImGuiKey_F7;
        case KeyboardKey::F8:
            return ImGuiKey_F8;
        case KeyboardKey::F9:
            return ImGuiKey_F9;
        case KeyboardKey::F10:
            return ImGuiKey_F10;
        case KeyboardKey::F11:
            return ImGuiKey_F11;
        case KeyboardKey::F12:
            return ImGuiKey_F12;

        case KeyboardKey::NumLock:
            return ImGuiKey_NumLock;
        case KeyboardKey::ScrollLock:
            return ImGuiKey_ScrollLock;

        case KeyboardKey::LeftShift:
            return ImGuiKey_LeftShift;
        case KeyboardKey::RightShift:
            return ImGuiKey_RightShift;
        case KeyboardKey::LeftControl:
            return ImGuiKey_LeftCtrl;
        case KeyboardKey::RightControl:
            return ImGuiKey_RightCtrl;
        case KeyboardKey::LeftAlt:
            return ImGuiKey_LeftAlt;
        case KeyboardKey::RightAlt:
            return ImGuiKey_RightAlt;

        case KeyboardKey::Semicolon:
            return ImGuiKey_Semicolon;

        case KeyboardKey::Equals:
            return ImGuiKey_Equal;
        case KeyboardKey::Comma:
            return ImGuiKey_Comma;
        case KeyboardKey::Minus:
            return ImGuiKey_Minus;
        case KeyboardKey::Period:
            return ImGuiKey_Period;

        case KeyboardKey::ForwardSlash:
            return ImGuiKey_Slash;

        case KeyboardKey::BackQuote:
            return ImGuiKey_GraveAccent;

        case KeyboardKey::LeftBracket:
            return ImGuiKey_LeftBracket;

        case KeyboardKey::BackSlash:
            return ImGuiKey_Backslash;

        case KeyboardKey::RightBracket:
            return ImGuiKey_RightBracket;

        case KeyboardKey::SingleQuote:
            return ImGuiKey_Apostrophe;

        default:
            return ImGuiKey_None;
        }
    }

	bool ImGuiInputLayer::HandleMouseStateChange(const Input::MouseStateChange& state, const Mouse& mouse)
	{
		InputService& input = *InputService::Get();

		ImGuiIO& io = ImGui::GetIO();

		// Add mouse events
		if (!io.WantSetMousePos && state.Position.has_value())
		{
			Vector2Int mousePos = state.Position.value();

			if ((io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) == 0)
			{
				ImGuiViewport* viewport = ImGui::GetMainViewport();
				mousePos.X -= static_cast<int>(viewport->Pos.x);
				mousePos.Y -= static_cast<int>(viewport->Pos.y);
			}

			io.AddMousePosEvent(static_cast<float>(mousePos.X), static_cast<float>(mousePos.Y));
		}

		if (state.Button.has_value())
		{
			MouseButton button = state.Button.value();

			io.AddMouseButtonEvent(static_cast<int>(button), state.IsButtonPressed);
		}

		if (state.ScrollDelta.has_value())
		{
			Vector2Int scrollDelta = state.ScrollDelta.value();

			io.AddMouseWheelEvent(static_cast<float>(scrollDelta.X), static_cast<float>(scrollDelta.Y));
		}

		return io.WantCaptureMouse;
	}

	bool ImGuiInputLayer::HandleKeyboardStateChange(const Input::KeyboardStateChange& state, const Keyboard& keyboard)
	{
		ImGuiIO& io = ImGui::GetIO();

		if (state.UnicodeKeyPoint.has_value())
		{
			io.AddInputCharacter(state.UnicodeKeyPoint.value());
		}

		if (state.Key.has_value())
		{
			io.AddKeyEvent(ImGuiMod_Ctrl, keyboard.IsKeyPressed(KeyboardKey::LeftControl) || keyboard.IsKeyPressed(KeyboardKey::RightControl));
			io.AddKeyEvent(ImGuiMod_Shift, keyboard.IsKeyPressed(KeyboardKey::LeftShift) || keyboard.IsKeyPressed(KeyboardKey::RightShift));
			io.AddKeyEvent(ImGuiMod_Alt, keyboard.IsKeyPressed(KeyboardKey::LeftAlt) || keyboard.IsKeyPressed(KeyboardKey::RightAlt));
			io.AddKeyEvent(ImGuiMod_Super, keyboard.IsKeyPressed(KeyboardKey::LeftMeta) || keyboard.IsKeyPressed(KeyboardKey::RightMeta));

			KeyboardKey key = state.Key.value();
			ImGuiKey imGuiKey = ToImGuiKey(key);

			io.AddKeyEvent(imGuiKey, state.IsPressed);
			io.SetKeyEventNativeData(imGuiKey, static_cast<int>(key), static_cast<int>(key));
		}

		return io.WantCaptureKeyboard;
	}
}