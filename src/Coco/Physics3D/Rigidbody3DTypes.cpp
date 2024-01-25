#include "Phys3Dpch.h"
#include "Rigidbody3DTypes.h"

namespace Coco::Physics3D
{
	Rigidbody3DCreateInfo::Rigidbody3DCreateInfo(
		const Vector3& position, 
		const Quaternion& rotation, 
		SharedRef<CollisionShape3D> collisionShape, 
		double mass, 
		Rigidbody3DType type) :
		Position(position),
		Rotation(rotation),
		CollisionShape(collisionShape),
		Mass(mass),
		Type(type)
	{}
}