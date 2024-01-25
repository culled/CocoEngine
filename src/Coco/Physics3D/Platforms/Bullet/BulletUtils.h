#pragma once

#include <Coco/Core/Types/Vector.h>
#include <Coco/Core/Types/Quaternion.h>

#include <btBulletDynamicsCommon.h>

namespace Coco::Physics3D::Bullet
{
	btTransform TobtTransform(const Vector3& position, const Quaternion& rotation);
	void DecomposebtTransform(const btTransform& transform, Vector3& outPosition, Quaternion& outRotation);

	btVector3 TobtVector3(const Vector3& vector);
	Vector3 ToVector3(const btVector3& vector);
}