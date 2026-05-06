//
// Created by cullen on 3/9/26.
//

#ifndef COCOENGINE_GRAPHICSRESOURCE_H
#define COCOENGINE_GRAPHICSRESOURCE_H
#include "Coco/Core/Types/CoreTypes.h"

namespace Coco
{
    class GraphicsResource
    {
    public:
        virtual ~GraphicsResource() = default;

        uint64 GetID() const { return _id; }

    protected:
        GraphicsResource(uint64 id);

    private:
        uint64 _id;
    };
} // Coco

#endif //COCOENGINE_GRAPHICSRESOURCE_H