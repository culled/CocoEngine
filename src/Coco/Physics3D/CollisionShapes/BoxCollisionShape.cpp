#include "Phys3Dpch.h"
#include "BoxCollisionShape.h"

namespace Coco::Physics3D
{
    BoxCollisionShape::BoxCollisionShape() :
        BoxCollisionShape(Vector3::One)
    {}

    BoxCollisionShape::BoxCollisionShape(const Vector3& size) :
        Size(size)
    {}
}