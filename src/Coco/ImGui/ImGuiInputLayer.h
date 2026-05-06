//
// Created by cullen on 4/6/26.
//

#ifndef COCOENGINE_IMGUIINPUTLAYER_H
#define COCOENGINE_IMGUIINPUTLAYER_H
#include "Coco/Input/InputLayer.h"
#include <imgui.h>

#include "Coco/Input/KeyboardTypes.h"

namespace Coco
{
    class ImGuiInputLayer : public InputLayer
    {
    public:
        static constexpr int Order = -100;

    public:
        int GetOrder() const override { return Order; }
        bool HandleInputEvent(const InputEvent& event) override;

    private:
        /// @brief Converts a KeyboardKey to an ImGuiKey
        /// @param key The KeyboardKey
        /// @return The ImGuiKey
        static ImGuiKey ToImGuiKey(KeyboardKey key) noexcept;

        /// @brief Converts a MouseButton to an ImGuiMouseButton
        /// @param button The MouseButton
        /// @return The ImGuiMouseButton
        static ImGuiMouseButton ToImGuiMouseButton(MouseButton button) noexcept;
    };
} // Coco

#endif //COCOENGINE_IMGUIINPUTLAYER_H