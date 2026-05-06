//
// Created by cullen on 3/1/26.
//

#ifndef COCOENGINE_INPUTLAYER_H
#define COCOENGINE_INPUTLAYER_H
#include "MouseTypes.h"

namespace Coco
{
    class InputLayer
    {
    public:
        virtual ~InputLayer() = default;

        virtual int GetOrder() const = 0;
        virtual bool HandleInputEvent(const InputEvent& event) = 0;
    };
} // Coco

#endif //COCOENGINE_INPUTLAYER_H