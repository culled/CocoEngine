//
// Created by cullen on 4/2/26.
//

#ifndef COCOENGINE_MESHRENDERERCOMPONENT_H
#define COCOENGINE_MESHRENDERERCOMPONENT_H
#include "Coco/Core/Memory/Ptrs.h"
#include "Coco/ECS/EntityComponent.h"

namespace Coco
{
    class Mesh;

    struct MeshRendererComponent : public EntityComponent
    {
        DECLARE_RTTI_TYPE(MeshRendererComponent)

    public:
        MeshRendererComponent(const UUID& ownerEntityID);

        SharedPtr<Mesh> RenderMesh;
    };
} // Coco

#endif //COCOENGINE_MESHRENDERERCOMPONENT_H