#pragma once
#include "../CollisionShape3D.h"

namespace Coco::Physics3D
{
    struct SphereCollisionShape :
        public CollisionShape3D
    {
        double Radius;

        SphereCollisionShape();
        SphereCollisionShape(double radius);
    };
}