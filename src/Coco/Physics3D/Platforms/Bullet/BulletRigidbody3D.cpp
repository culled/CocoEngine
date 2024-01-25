#include "Phys3Dpch.h"
#include "BulletRigidbody3D.h"

#include "../../CollisionShapes/BoxCollisionShape.h"
#include "../../CollisionShapes/SphereCollisionShape.h"
#include "BulletUtils.h"

namespace Coco::Physics3D::Bullet
{
	BulletRigidbody3D::BulletRigidbody3D(const Rigidbody3DCreateInfo& createInfo) :
		_localInertia(0.f, 0.f, 0.f),
		_baseShapes(),
		_mass(createInfo.Mass),
		_type(createInfo.Type)
	{
		btTransform transform = TobtTransform(createInfo.Position, createInfo.Rotation);

		_motionState = CreateUniqueRef<btDefaultMotionState>(transform);

		std::array<SharedRef<CollisionShape3D>, 1> shapes = { createInfo.CollisionShape };
		BuildCollisionShape(shapes);

		float massToUse = _type == Rigidbody3DType::Static ? 0.f : static_cast<float>(_mass);

		if (_type == Rigidbody3DType::Dynamic)
			_collisionShape->calculateLocalInertia(massToUse, _localInertia);


		btRigidBody::btRigidBodyConstructionInfo rbInfo(massToUse, _motionState.get(), _collisionShape.get(), _localInertia);
		_body = CreateUniqueRef<btRigidBody>(rbInfo);

		UpdateBodyState();
	}

	BulletRigidbody3D::~BulletRigidbody3D()
	{
		_body.reset();
		_motionState.reset();
		_collisionShape.reset();
		_baseShapes.clear();
	}

	void BulletRigidbody3D::SetMass(double mass)
	{
		_mass = mass;

		if (_type != Rigidbody3DType::Static)
			_body->setMassProps(static_cast<float>(mass), _localInertia);
	}

	void BulletRigidbody3D::SetType(Rigidbody3DType type)
	{
		_type = type;

		if (type != Rigidbody3DType::Dynamic)
			_body->setMassProps(0.f, _localInertia);

		UpdateBodyState();
	}

	void BulletRigidbody3D::SetTransform(const Vector3& position, const Quaternion& rotation)
	{
		if (_type == Rigidbody3DType::Static)
			return;

		_motionState->setWorldTransform(TobtTransform(position, rotation));
	}

	void BulletRigidbody3D::GetTransform(Vector3& outPosition, Quaternion& outRotation, bool withInterpolation) const
	{
		btTransform transform;

		if (withInterpolation)
		{
			_motionState->getWorldTransform(transform);
		}
		else
		{
			transform = _body->getWorldTransform();
		}

		outPosition = ToVector3(transform.getOrigin());

		btQuaternion r = transform.getRotation();

		outRotation.X = r.x();
		outRotation.Y = r.y();
		outRotation.Z = r.z();
		outRotation.W = r.w();
	}

	bool BulletRigidbody3D::IsAwake() const
	{
		int state = _body->getActivationState();
		return state == ACTIVE_TAG || state == WANTS_DEACTIVATION;
	}

	Vector3 BulletRigidbody3D::GetLinearVelocity() const
	{
		return ToVector3(_body->getLinearVelocity());
	}

	Vector3 BulletRigidbody3D::GetAngularVelocity() const
	{
		return ToVector3(_body->getAngularVelocity());
	}

	void BulletRigidbody3D::BuildCollisionShape(std::span<SharedRef<CollisionShape3D>> collisionShapes)
	{
		_collisionShape.reset();
		_baseShapes.clear();

		if (collisionShapes.size() == 1)
		{
			SharedRef<CollisionShape3D>& shape = collisionShapes.front();

			if (const BoxCollisionShape* boxShape = dynamic_cast<const BoxCollisionShape*>(shape.get()))
			{
				_collisionShape = CreateUniqueRef<btBoxShape>(TobtVector3(boxShape->Size * 0.5));
			}
			else if (const SphereCollisionShape* sphereShape = dynamic_cast<const SphereCollisionShape*>(shape.get()))
			{
				_collisionShape = CreateUniqueRef<btSphereShape>(static_cast<btScalar>(sphereShape->Radius));
			}
		}
		//else
		//{
		//	UniqueRef<btCompoundShape> compoundShape = CreateUniqueRef<btCompoundShape>(true, static_cast<int>(collisionShapes.size()));
		//
		//	for (const CollisionShape3D* shape : collisionShapes)
		//	{
		//		compoundShape->addChildShape()
		//	}
		//
		//	_collisionShape = std::move(compoundShape);
		//}
	}

	void BulletRigidbody3D::UpdateBodyState()
	{
		int collisionFlags = _body->getCollisionFlags();
		int activationState = _body->getActivationState();

		switch (_type)
		{
		case Rigidbody3DType::Dynamic:
		{
			collisionFlags &= ~btCollisionObject::CollisionFlags::CF_KINEMATIC_OBJECT;

			if(activationState == DISABLE_DEACTIVATION)
				activationState = WANTS_DEACTIVATION;
			break;
		}
		case Rigidbody3DType::Kinematic:
		{			
			collisionFlags |= btCollisionObject::CollisionFlags::CF_KINEMATIC_OBJECT;
			activationState = DISABLE_DEACTIVATION;
			break;
		}
		default:
			break;
		}

		_body->setCollisionFlags(collisionFlags);
		_body->setActivationState(activationState);
	}
}