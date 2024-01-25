#include "ECSpch.h"
#include "Rigidbody3DComponent.h"

using namespace Coco::Physics3D;

namespace Coco::ECS
{
	Rigidbody3DComponent::Rigidbody3DComponent(const Entity& owner) :
		Rigidbody3DComponent(owner, 10, Rigidbody3DType::Dynamic)
	{}

	Rigidbody3DComponent::Rigidbody3DComponent(const Entity & owner, double mass, Physics3D::Rigidbody3DType type) :
		EntityComponent(owner),
		_mass(mass),
		_type(type),
		_interpolate(true)
	{}

	void Rigidbody3DComponent::SetMass(double mass)
	{
		_mass = mass;

		if (_body)
			_body->SetMass(mass);
	}

	void Rigidbody3DComponent::SetType(Physics3D::Rigidbody3DType type)
	{
		_type = type;

		if (_body)
			_body->SetType(type);
	}

	void Rigidbody3DComponent::GetWorldTransform(Vector3& outPosition, Quaternion& outRotation) const
	{
		if (_body)
			_body->GetTransform(outPosition, outRotation, _interpolate);
	}

	bool Rigidbody3DComponent::IsAwake() const
	{
		if (!_body)
			return false;

		return _body->IsAwake();
	}

	Vector3 Rigidbody3DComponent::GetLinearVelocity() const
	{
		if(!_body)
			return Vector3::Zero;

		return _body->GetLinearVelocity();
	}

	Vector3 Rigidbody3DComponent::GetAngularVelocity() const
	{
		if (!_body)
			return Vector3::Zero;

		return _body->GetAngularVelocity();
	}
}