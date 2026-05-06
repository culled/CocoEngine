//
// Created by cullen on 3/14/26.
//
#include "RenderSceneTypes.h"

namespace Coco
{
    RenderObject::RenderObject(uint64 id, uint64 layer, uint64 meshID, const Submesh& drawSubmesh) :
        ID(id),
        Layer(layer),
        MeshID(meshID),
        DrawSubmesh(drawSubmesh)
    {}
}
