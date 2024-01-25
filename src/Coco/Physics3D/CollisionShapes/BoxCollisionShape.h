#pragma once
#include "../CollisionShape3D.h"

#include <Coco/Core/Types/Vector.h>

namespace Coco::Physics3D
{
    struct BoxCollisionShape :
        public CollisionShape3D
    {
        Vector3 Size;

        BoxCollisionShape();
        BoxCollisionShape(const Vector3& size);
    };
}