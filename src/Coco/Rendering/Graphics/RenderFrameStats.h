//
// Created by cullen on 4/9/26.
//

#ifndef COCOENGINE_RENDERFRAMESTATS_H
#define COCOENGINE_RENDERFRAMESTATS_H
#include "Coco/Core/Types/CoreTypes.h"

namespace Coco
{
    struct RenderFrameStats
    {
        uint64 TrianglesDrawn;
        uint64 VerticesDrawn;
        uint64 DrawCalls;
        uint64 MemoryUsage;
    };
} // Coco

#endif //COCOENGINE_RENDERFRAMESTATS_H