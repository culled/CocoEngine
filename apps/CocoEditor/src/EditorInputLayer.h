#pragma once
#include <Coco\Input\InputLayer.h>

namespace Coco
{
    class EditorInputLayer :
        public Input::InputLayer
    {
    public:
        static const int Priority;

    public:
        // Inherited via InputLayer
        int GetPriority() const override { return Priority; }
        bool HandleKeyboardStateChange(const Input::KeyboardStateChange& state, const Input::Keyboard& keyboard) override;
    };
}