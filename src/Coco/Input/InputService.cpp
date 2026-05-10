//
// Created by cullen on 3/1/26.
//

#include "InputService.h"
#include <Coco/Core/Engine.h>

#include "Coco/Core/Types/Sorting/QSorter.h"

namespace Coco
{
    InputService::InputService(Engine* engine) :
        EngineService(engine),
        _inputPlatform(dynamic_cast<InputEnginePlatform*>(engine->GetPlatform())),
        _mouse(),
        _keyboard(),
        _savePreviousStateTickListener(this, &InputService::HandleSavePreviousStateTick, SavePreviousStateTickOrder),
        _inputLayers(nullptr, 4),
        _inputLayersNeedSorting(false),
        _currentLayerIndex(0)
    {
        if (!_inputPlatform)
            throw Exception("Platform does not support input");

        if (_inputPlatform->SupportsMouse())
            _mouse = CreateDefaultUnique<Mouse>(this);

        if (_inputPlatform->SupportsKeyboard())
            _keyboard = CreateDefaultUnique<Keyboard>(this);

        _savePreviousStateTickListener.ListenTo(*engine->GetMainLoop());

        COCO_ENGINE_LOG_VERBOSE("Created InputService");
    }

    InputService::~InputService()
    {
        _savePreviousStateTickListener.StopListening();
        _inputLayers.Clear(true);
        _keyboard.reset();
        _mouse.reset();

        COCO_ENGINE_LOG_VERBOSE("Destroyed InputService");
    }

    void InputService::DispatchInputEvent(const InputEvent& inputEvent)
    {
        if (_inputLayersNeedSorting)
            SortInputLayers();

        try
        {
            for (_currentLayerIndex = 0; _currentLayerIndex < static_cast<int64>(_inputLayers.GetCount()); ++_currentLayerIndex)
            {
                bool handled = _inputLayers[_currentLayerIndex]->HandleInputEvent(inputEvent);
                if (handled)
                    break;
            }
        }
        catch (const std::exception& ex)
        {
            COCO_ENGINE_LOG_ERROR("Exception caught while dispatching input event: %s", ex.what());
        }
        catch (...)
        {
            COCO_ENGINE_LOG_ERROR("Unknown exception caught while dispatching input event to layer %d", _currentLayerIndex);
        }
    }

    void InputService::ResetState()
    {
        if (_mouse)
            _mouse->ResetState();

        if (_keyboard)
            _keyboard->ResetState();
    }

    void InputService::AddInputLayer(SharedPtr<InputLayer> inputLayer)
    {
        _inputLayers.Append(inputLayer);
        _inputLayersNeedSorting = true;
    }

    void InputService::RemoveInputLayer(const InputLayer& inputLayer)
    {
        int64 index = _inputLayers.Find(
            [layerToRemove = &inputLayer](const SharedPtr<InputLayer>& otherLayer)
            {
                return otherLayer.get() == layerToRemove;
            });

        if (index == -1)
            return;

        _inputLayers.RemoveAt(index);

        if (_currentLayerIndex >= index)
            --_currentLayerIndex;
    }

    void InputService::HandleSavePreviousStateTick(const TickInfo& tickInfo)
    {
        if (_mouse)
            _mouse->SavePreviousState();

        if (_keyboard)
            _keyboard->SavePreviousState();
    }

    void InputService::SortInputLayers()
    {
        QSorter<SharedPtr<InputLayer>> sorter(
            [](const SharedPtr<InputLayer>& a, const SharedPtr<InputLayer>& b) { return a->GetOrder() < b->GetOrder(); });

        sorter.Sort(_inputLayers);
        _inputLayersNeedSorting = false;
    }
} // Coco