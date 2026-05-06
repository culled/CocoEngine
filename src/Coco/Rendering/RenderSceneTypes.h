//
// Created by cullen on 3/13/26.
//

#ifndef COCOENGINE_RENDERSCENETYPES_H
#define COCOENGINE_RENDERSCENETYPES_H
#include <Coco/Core/Types/CoreTypes.h>

#include "Mesh.h"

namespace Coco
{
    struct RenderObject
    {
        uint64 ID;
        uint64 Layer;
        uint64 MeshID;
        Submesh DrawSubmesh;

        RenderObject(uint64 id, uint64 layer, uint64 meshID, const Submesh& drawSubmesh);
    };
}
#endif //COCOENGINE_RENDERSCENETYPES_H