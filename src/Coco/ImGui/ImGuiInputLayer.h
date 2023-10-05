#pragma once
#include <Coco\Input\InputLayer.h>

using namespace Coco::Input;

enum ImGuiKey;

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
        static ImGuiKey ToImGuiKey(KeyboardKey key);

        int GetPriority() const final { return Priority; }
        bool HandleMouseStateChange(const MouseStateChange& state, const Mouse& mouse) final;
        bool HandleKeyboardStateChange(const KeyboardStateChange& state, const Keyboard& keyboard) final;
    };
}