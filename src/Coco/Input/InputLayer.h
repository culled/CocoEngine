//
// Created by cullen on 3/1/26.
//

#ifndef COCOENGINE_INPUTLAYER_H
#define COCOENGINE_INPUTLAYER_H
#include "MouseTypes.h"

namespace Coco
{
    /// @brief Base class for a class that can be registered to the InputService and receive input events as they occur
    class InputLayer
    {
    public:
        virtual ~InputLayer() = default;

        /// @brief Gets the order of this layer. Input layers are ordered in ascending order
        /// @return The order of this layer
        virtual int GetOrder() const = 0;

        /// @brief Called by the InputService when dispatching an input event
        /// @param event The input event
        /// @return True if the event should not be dispatched to later input layers
        virtual bool HandleInputEvent(const InputEvent& event) = 0;
    };
} // Coco

#endif //COCOENGINE_INPUTLAYER_H