#pragma once
#include <Coco\Input\InputLayer.h>

namespace Coco::ImGuiCoco
{
    /// @brief An input layer for ImGui
    class ImGuiInputLayer :
        public Input::InputLayer
    {
    public:
        /// @brief The priority of this input layer
        static const int Priority;

    public:
        int GetPriority() const final { return Priority; }
        bool HandleMouseStateChange(const Input::MouseStateChange& state) final;
        bool HandleKeyboardStateChange(const Input::KeyboardStateChange& state) final;
    };
}