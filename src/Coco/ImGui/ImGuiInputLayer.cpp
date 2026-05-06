//
// Created by cullen on 4/6/26.
//

#include "ImGuiInputLayer.h"

#include "Coco/Core/Engine.h"
#include "Coco/Windowing/Window.h"

namespace Coco
{
    bool ImGuiInputLayer::HandleInputEvent(const InputEvent& event)
    {
        ImGuiIO& io = ImGui::GetIO();

        switch (event.GetType())
        {
            case KeyboardKeyInputEvent::Type:
            {
                const KeyboardKeyInputEvent& keyEvent = static_cast<const KeyboardKeyInputEvent&>(event);
                io.AddKeyEvent(ImGuiMod_Ctrl, (keyEvent.ModifierKeys & KeyboardModifierKeyFlags::Control) == KeyboardModifierKeyFlags::Control);
                io.AddKeyEvent(ImGuiMod_Shift, (keyEvent.ModifierKeys & KeyboardModifierKeyFlags::Shift) == KeyboardModifierKeyFlags::Shift);
                io.AddKeyEvent(ImGuiMod_Alt, (keyEvent.ModifierKeys & KeyboardModifierKeyFlags::Alt) == KeyboardModifierKeyFlags::Alt);
                io.AddKeyEvent(ImGuiMod_Super, (keyEvent.ModifierKeys & KeyboardModifierKeyFlags::Meta) == KeyboardModifierKeyFlags::Meta);

                ImGuiKey imGuiKey = ToImGuiKey(keyEvent.Key);

                io.AddKeyEvent(imGuiKey, keyEvent.IsPressed);
                io.SetKeyEventNativeData(imGuiKey, static_cast<int>(keyEvent.Key), static_cast<int>(keyEvent.Key));

                return io.WantCaptureKeyboard;
            }
            case MouseButtonInputEvent::Type:
            {
                const MouseButtonInputEvent& mouseEvent = static_cast<const MouseButtonInputEvent&>(event);
                ImGuiMouseButton mouseButton = ToImGuiMouseButton(mouseEvent.Button);
                io.AddMouseButtonEvent(mouseButton, mouseEvent.IsPressed);

                return io.WantCaptureMouse;
            }
            case MouseMoveInputEvent::Type:
            {
                const MouseMoveInputEvent& mouseEvent = static_cast<const MouseMoveInputEvent&>(event);

                if (!io.WantSetMousePos)
                {
                    if ((io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) == 0)
                    {
                        ImGuiViewport* viewport = ImGui::GetMainViewport();
                        Window* viewportWindow = static_cast<Window*>(viewport->PlatformHandle);
                        COCO_ASSERT(viewportWindow, "Window was null");

                        Vector2i windowPos = viewportWindow->GetPosition(false);
                        io.AddMousePosEvent(
                            static_cast<float>(mouseEvent.NewPosition.X() - windowPos.X()),
                            static_cast<float>(mouseEvent.NewPosition.Y() - windowPos.Y())
                        );
                    }
                    else
                    {
                        io.AddMousePosEvent(static_cast<float>(mouseEvent.NewPosition.X()), static_cast<float>(mouseEvent.NewPosition.Y()));
                    }
                }

                return io.WantCaptureMouse;
            }
            case MouseScrollInputEvent::Type:
            {
                const MouseScrollInputEvent& mouseEvent = static_cast<const MouseScrollInputEvent&>(event);
                io.AddMouseWheelEvent(static_cast<float>(mouseEvent.Delta.X()), static_cast<float>(mouseEvent.Delta.Y()));

                return io.WantCaptureMouse;
            }
            case MouseRawMoveInputEvent::Type:
            {
                return io.WantCaptureMouse;
            }
            default:
                return false;
        }
    }

    ImGuiKey ImGuiInputLayer::ToImGuiKey(KeyboardKey key) noexcept
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

    ImGuiMouseButton ImGuiInputLayer::ToImGuiMouseButton(MouseButton button) noexcept
    {
        switch (button)
        {
        case MouseButton::Left:
            return ImGuiMouseButton_Left;
        case MouseButton::Middle:
            return ImGuiMouseButton_Middle;
        case MouseButton::Right:
            return ImGuiMouseButton_Right;
        case MouseButton::Button3:
            return 3;
        case MouseButton::Button4:
            return 4;
        default:
            return 0;
        }
    }
} // Coco