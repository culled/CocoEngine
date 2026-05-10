//
// Created by cullen on 3/1/26.
//

#ifndef COCOENGINE_INPUTSERVICE_H
#define COCOENGINE_INPUTSERVICE_H
#include "InputEvent.h"
#include "Coco/Core/EngineService.h"
#include "Coco/Core/Memory/Ptrs.h"
#include <Coco/Core/ProcessLoop/TickListener.h>
#include "Coco/Core/Types/Array.h"

#include "InputLayer.h"
#include "InputEnginePlatform.h"
#include "Mouse.h"
#include "Keyboard.h"

namespace Coco
{
    /// @brief An EngineService that manages input from input devices
    class InputService : public EngineService
    {
    public:
        /// @brief The tick order for saving previous input state
        static constexpr int SavePreviousStateTickOrder = 10000;

        InputService(Engine* engine);
        ~InputService();

        /// @brief Determines if the platform supports mouse input
        /// @return True if the platform supports mouse input
        bool SupportsMouseInput() const { return _inputPlatform->SupportsMouse(); }

        /// @brief Determines if the platform supports keyboard input
        /// @return True if the platform supports keyboard input
        bool SupportsKeyboardInput() const { return _inputPlatform->SupportsKeyboard(); }

        /// @brief Determines if the platform supports raw input
        /// @return True if the platform supports raw input
        bool SupportsRawInput() const { return _inputPlatform->SupportsRawInput(); }

        /// @brief Dispatches the given event to all input layers
        /// @param inputEvent The input event
        void DispatchInputEvent(const InputEvent& inputEvent);

        /// @brief Gets the mouse device. Only valid if the platform supports mouse input
        /// @return The mouse, or nullptr if the platform does not support mouse input
        Mouse* GetMouse() { return _mouse.get(); }

        /// @brief Gets the mouse device. Only valid if the platform supports mouse input
        /// @return The mouse, or nullptr if the platform does not support mouse input
        const Mouse* GetMouse() const { return _mouse.get(); }

        /// @brief Gets the keyboard device. Only valid if the platform supports keyboard input
        /// @return The keyboard, or nullptr if the platform does not support keyboard input
        Keyboard* GetKeyboard() { return _keyboard.get(); }

        /// @brief Gets the keyboard device. Only valid if the platform supports keyboard input
        /// @return The keyboard, or nullptr if the platform does not support keyboard input
        const Keyboard* GetKeyboard() const { return _keyboard.get(); }

        /// @brief Resets the state of all input
        void ResetState();

        /// @brief Adds an input layer that will be notified of input events
        /// @param inputLayer The input layer
        void AddInputLayer(SharedPtr<InputLayer> inputLayer);

        /// @brief Removes an input layer, causing it to not be notified of input events anymore
        /// @param inputLayer The input layer
        void RemoveInputLayer(const InputLayer& inputLayer);

    private:
        InputEnginePlatform* _inputPlatform;
        UniquePtr<Mouse> _mouse;
        UniquePtr<Keyboard> _keyboard;
        TickListener _savePreviousStateTickListener;
        Array<SharedPtr<InputLayer>> _inputLayers;
        bool _inputLayersNeedSorting;
        int64 _currentLayerIndex;

        /// @brief Tick handler for saving the previous input state
        /// @param tickInfo The tick info
        void HandleSavePreviousStateTick(const TickInfo& tickInfo);

        /// @brief Sorts all input layers based on their order
        void SortInputLayers();
    };
} // Coco

#endif //COCOENGINE_INPUTSERVICE_H