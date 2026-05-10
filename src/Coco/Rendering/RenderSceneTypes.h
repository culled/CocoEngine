//
// Created by cullen on 3/13/26.
//

#ifndef COCOENGINE_RENDERSCENETYPES_H
#define COCOENGINE_RENDERSCENETYPES_H
#include <Coco/Core/Types/CoreTypes.h>

#include "Mesh.h"

namespace Coco
{
    /// @brief An individual object that can be rendered
    struct RenderObject
    {
        /// @brief The object's ID
        uint64 ID;

        /// @brief Layers used to filter the object. Usually used as a bitmask
        uint64 Layer;

        /// @brief The ID of the mesh this object uses for rendering
        uint64 MeshID;

        /// @brief The submesh of this object
        Submesh DrawSubmesh;

        RenderObject(uint64 id, uint64 layer, uint64 meshID, const Submesh& drawSubmesh);
    };
}
#endif //COCOENGINE_RENDERSCENETYPES_H