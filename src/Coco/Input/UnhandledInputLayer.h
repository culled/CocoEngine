#pragma once
#include "InputLayer.h"
#include <Coco/Core/Events/Event.h>

#include <Coco/Core/Math/Math.h>

namespace Coco::Input
{
    /// @brief An input layer that can handle input that no other layers consumed
    class UnhandledInputLayer :
        public InputLayer
    {
    public:
        /// @brief Invoked when the mouse's state changes
        Event<const MouseStateChange&> OnMouseStateChanged;

        /// @brief Invoked when the keyboard's state changes
        Event<const KeyboardStateChange&> OnKeyboardStateChanged;

    public:
        int GetPriority() const final { return Math::MaxValue<int>(); }
        bool HandleMouseStateChange(const MouseStateChange& state) final;
        bool HandleKeyboardStateChange(const KeyboardStateChange& state) final;
    };
}