#include "Phys3Dpch.h"
#include "BulletUtils.h"

namespace Coco::Physics3D::Bullet
{
	btTransform TobtTransform(const Vector3& position, const Quaternion& rotation)
	{
		return btTransform(
			btQuaternion(static_cast<float>(rotation.X), static_cast<float>(rotation.Y), static_cast<float>(rotation.Z), static_cast<float>(rotation.W)),
			TobtVector3(position)
		);
	}

	void DecomposebtTransform(const btTransform& transform, Vector3& outPosition, Quaternion& outRotation)
	{
		btQuaternion rotation = transform.getRotation();

		outPosition = ToVector3(transform.getOrigin());
		outRotation = Quaternion(rotation.x(), rotation.y(), rotation.z(), rotation.w());
	}

	btVector3 TobtVector3(const Vector3& vector)
	{
		return btVector3(static_cast<float>(vector.X), static_cast<float>(vector.Y), static_cast<float>(vector.Z));
	}

	Vector3 ToVector3(const btVector3& vector)
	{
		return Vector3(vector.x(), vector.y(), vector.z());
	}
}