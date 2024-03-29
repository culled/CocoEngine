#pragma once
#include <Coco\Input\InputLayer.h>

using namespace Coco::Input;

enum ImGuiKey;
typedef int ImGuiMouseButton;

namespace Coco::ImGuiCoco
{
    /// @brief An input layer for ImGui
    class ImGuiInputLayer :
        public InputLayer
    {
    public:
        /// @brief The priority of this input layer
        static const int Priority;

    public:
        /// @brief Converts a keyboard key to an ImGuiKey
        /// @param key The key
        /// @return The equivalent ImGuiKey
        static ImGuiKey ToImGuiKey(KeyboardKey key);

        /// @brief Converts a mouse button to an ImGuiMouseButton
        /// @param button The mouse button
        /// @return The equivalent ImGuiMouseButton
        static ImGuiMouseButton ToImGuiMouseButton(MouseButton button);

        int GetPriority() const final { return Priority; }
        bool HandleMouseStateChange(const MouseStateChange& state, const Mouse& mouse) final;
        bool HandleKeyboardStateChange(const KeyboardStateChange& state, const Keyboard& keyboard) final;
    };
}