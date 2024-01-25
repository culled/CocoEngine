#include "Phys3Dpch.h"
#include "SphereCollisionShape.h"

namespace Coco::Physics3D
{
	SphereCollisionShape::SphereCollisionShape() :
		SphereCollisionShape(0.5)
	{}

	SphereCollisionShape::SphereCollisionShape(double radius) :
		Radius(radius)
	{}
}