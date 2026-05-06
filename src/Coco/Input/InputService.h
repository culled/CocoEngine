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
    class InputService : public EngineService
    {
    public:
        /// @brief The tick order for saving previous input state
        static constexpr int SavePreviousStateTickOrder = 10000;

    public:
        InputService(Engine* engine);
        ~InputService();

        void DispatchInputEvent(const InputEvent& inputEvent);

        Mouse* GetMouse() { return _mouse.get(); }
        const Mouse* GetMouse() const { return _mouse.get(); }

        Keyboard* GetKeyboard() { return _keyboard.get(); }
        const Keyboard* GetKeyboard() const { return _keyboard.get(); }

        /// @brief Resets the state of all input
        void ResetState();

        void AddInputLayer(SharedPtr<InputLayer> inputLayer);
        void RemoveInputLayer(const InputLayer& layer);

        bool SupportsRawInput() const;

    private:
        InputEnginePlatform* _inputPlatform;
        UniquePtr<Mouse> _mouse;
        UniquePtr<Keyboard> _keyboard;
        TickListener _savePreviousStateTickListener;
        Array<SharedPtr<InputLayer>> _inputLayers;
        bool _inputLayersNeedSorting;
        int64 _currentLayerIndex;

    private:
        void HandleSavePreviousStateTick(const TickInfo& tickInfo);
        void SortInputLayers();
    };
} // Coco

#endif //COCOENGINE_INPUTSERVICE_H