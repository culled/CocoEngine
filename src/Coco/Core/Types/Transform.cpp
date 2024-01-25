#include "Corepch.h"
#include "Transform.h"

namespace Coco
{
	Transform2D::Transform2D() :
		Transform2D(Vector2::Zero, 0.0, Vector2::One)
	{}

	Transform2D::Transform2D(const Vector2& position, double rotation, const Vector2& scale, const Transform2D* parent) :
		LocalPosition(position),
		LocalRotation(rotation),
		LocalScale(scale),
		LocalTransform(),
		GlobalTransform(),
		InvGlobalTransform()
	{
		Recalculate(parent);
	}

	void Transform2D::Recalculate(const Transform2D* parent)
	{
		LocalTransform = Matrix4x4::CreateTransform(LocalPosition, Quaternion(Vector3(0.0, 0.0, LocalRotation)), LocalScale);

		if (parent)
		{
			ParentTransform = parent->GlobalTransform;
			InvParentTransform = parent->InvGlobalTransform;
		}
		else
		{
			ParentTransform = Matrix4x4::Identity;
			InvParentTransform = Matrix4x4::Identity;
		}

		GlobalTransform = LocalTransform * ParentTransform;
		InvGlobalTransform = GlobalTransform.Inverted();
	}

	Vector2 Transform2D::TransformPosition(const Vector2& position, TransformSpace from, TransformSpace to) const
	{
		if (from == to)
			return position;

		Vector4 v = GetTransformMatrix(from, to) * Vector4(position, 0.0, 1.0);
		return v.XY();
	}

	double Transform2D::TransformRotation(double rotation, TransformSpace from, TransformSpace to) const
	{
		if (from == to)
			return rotation;

		Quaternion r(Vector3(0.0, 0.0, rotation));
		Quaternion otherRotation = GetTransformMatrix(from, to).GetRotation();
		Quaternion result = from < to ? otherRotation * r : r * otherRotation;

		return result.ToEulerAngles().Z;
	}

	Vector2 Transform2D::TransformVector(const Vector2& vector, TransformSpace from, TransformSpace to) const
	{
		if (from == to)
			return vector;

		Vector4 v = GetTransformMatrix(from, to) * Vector4(vector, 0.0, 0.0);
		return v.XY();
	}

	Vector2 Transform2D::TransformScale(const Vector2& scale, TransformSpace from, TransformSpace to) const
	{
		if (from == to)
			return scale;

		// The scale will be rotated in the transform conversion, so undo the rotation once it's complete
		Quaternion rotation = GetTransformMatrix(from, to).GetRotation().Inverted();
		return (rotation * Vector3(TransformVector(scale, from, to))).XY();
	}

	void Transform2D::Translate(const Vector2& translation, TransformSpace space)
	{
		Vector3 movement = Quaternion(Vector3(0.0, 0.0, GetRotation(space))) * Vector3(translation);

		LocalPosition += TransformVector(movement.XY(), space, TransformSpace::Parent);
	}

	void Transform2D::Rotate(double rotation, TransformSpace space)
	{
		LocalRotation += TransformRotation(rotation, space, TransformSpace::Parent);
	}

	void Transform2D::SetPosition(const Vector2& position, TransformSpace space)
	{
		LocalPosition = TransformPosition(position, space, TransformSpace::Parent);
	}

	Vector2 Transform2D::GetPosition(TransformSpace space) const
	{
		return TransformPosition(LocalPosition, TransformSpace::Parent, space);
	}

	void Transform2D::SetRotation(double rotation, TransformSpace space)
	{
		LocalRotation = TransformRotation(rotation, space, TransformSpace::Parent);
	}

	double Transform2D::GetRotation(TransformSpace space) const
	{
		return TransformRotation(LocalRotation, TransformSpace::Parent, space);
	}

	void Transform2D::SetScale(const Vector2& scale, TransformSpace space)
	{
		LocalScale = TransformScale(scale, space, TransformSpace::Parent);
	}

	Vector2 Transform2D::GetScale(TransformSpace space) const
	{
		return TransformScale(LocalScale, TransformSpace::Parent, space);
	}

	void Transform2D::Decompose(TransformSpace space, Vector2& outPosition, double& outRotation, Vector2& outScale) const
	{
		outPosition = GetPosition(space);
		outRotation = GetRotation(space);
		outScale = GetScale(space);
	}

	void Transform2D::Transform(TransformSpace from, TransformSpace to, Vector2& position, double& rotation, Vector2& scale) const
	{
		position = TransformPosition(position, from, to);
		rotation = TransformRotation(rotation, from, to);
		scale = TransformScale(scale, from, to);
	}

	Vector2 Transform2D::GetRight(TransformSpace space) const
	{
		return TransformVector(Vector2::Right, TransformSpace::Self, space);
	}

	Vector2 Transform2D::GetLeft(TransformSpace space) const
	{
		return TransformVector(Vector2::Left, TransformSpace::Self, space);
	}

	Vector2 Transform2D::GetUp(TransformSpace space) const
	{
		return TransformVector(Vector2::Up, TransformSpace::Self, space);
	}

	Vector2 Transform2D::GetDown(TransformSpace space) const
	{
		return TransformVector(Vector2::Down, TransformSpace::Self, space);
	}

	Matrix4x4 Transform2D::GetTransformMatrix(TransformSpace from, TransformSpace to) const
	{
		if (from == to)
		{
			return Matrix4x4::Identity;
		}

		switch (from)
		{
		case TransformSpace::Global:
		{
			switch (to)
			{
			case TransformSpace::Parent:
				return InvParentTransform;
			case TransformSpace::Self:
				return InvGlobalTransform;
			default:
				break;
			}
			break;
		}
		case TransformSpace::Parent:
		{
			switch (to)
			{
			case TransformSpace::Global:
				return ParentTransform;
			case TransformSpace::Self:
				return LocalTransform.Inverted();
			default:
				break;
			}
			break;
		}
		case TransformSpace::Self:
		{
			switch (to)
			{
			case TransformSpace::Global:
				return GlobalTransform;
			case TransformSpace::Parent:
				return LocalTransform;
			default:
				break;
			}
			break;
		}
		}

		Assert(false)
		return Matrix4x4::Identity;
	}

	Transform3D::Transform3D() : 
		Transform3D(Vector3::Zero, Quaternion::Identity, Vector3::One)
	{}

	Transform3D::Transform3D(const Vector3& position, const Quaternion& rotation, const Vector3& scale, const Transform3D* parent) :
		LocalPosition(position),
		LocalRotation(rotation),
		LocalScale(scale),
		LocalTransform(true),
		ParentTransform(true),
		InvParentTransform(true),
		GlobalTransform(true),
		InvGlobalTransform(true)
	{
		Recalculate(parent);
	}

	void Transform3D::Recalculate(const Transform3D* parent)
	{
		LocalTransform = Matrix4x4::CreateTransform(LocalPosition, LocalRotation, LocalScale);

		if (parent)
		{
			ParentTransform = parent->GlobalTransform;
			InvParentTransform = parent->InvGlobalTransform;
		}
		else
		{
			ParentTransform = Matrix4x4::Identity;
			InvParentTransform = Matrix4x4::Identity;
		}

		GlobalTransform = LocalTransform * ParentTransform;
		InvGlobalTransform = GlobalTransform.Inverted();
	}

	Vector3 Transform3D::TransformPosition(const Vector3& position, TransformSpace from, TransformSpace to) const
	{
		if (from == to)
			return position;
		
		Vector4 v = GetTransformMatrix(from, to) * Vector4(position, 1.0);
		return v.XYZ();
	}

	Quaternion Transform3D::TransformRotation(const Quaternion& rotation, TransformSpace from, TransformSpace to) const
	{
		if (from == to)
			return rotation;

		Quaternion otherRotation = GetTransformMatrix(from, to).GetRotation();

		if(from < to)
			return otherRotation * rotation;
		else
			return rotation * otherRotation;
	}

	Vector3 Transform3D::TransformVector(const Vector3& vector, TransformSpace from, TransformSpace to) const
	{
		if (from == to)
			return vector;

		Vector4 v = GetTransformMatrix(from, to) * Vector4(vector, 0.0);
		return v.XYZ();
	}

	Vector3 Transform3D::TransformScale(const Vector3& scale, TransformSpace from, TransformSpace to) const
	{
		if (from == to)
			return scale;

		// The scale will be rotated in the transform conversion, so undo the rotation once it's complete
		Quaternion rotation = GetTransformMatrix(from, to).GetRotation().Inverted();
		return rotation * TransformVector(scale, from, to);
	}

	void Transform3D::Translate(const Vector3& translation, TransformSpace space)
	{
		Vector3 movement = GetRotation(space) * translation;

		LocalPosition += TransformVector(movement, space, TransformSpace::Parent);
	}

	void Transform3D::Rotate(const Quaternion& rotation, TransformSpace space)
	{
		LocalRotation *= TransformRotation(rotation, space, TransformSpace::Parent);
	}

	void Transform3D::Rotate(const Vector3& eulerAngles, TransformSpace space)
	{
		Rotate(Quaternion(eulerAngles), space);
	}

	void Transform3D::Rotate(const Vector3& axis, double angleRadians, TransformSpace space)
	{
		Rotate(Quaternion(axis, angleRadians), space);
	}

	void Transform3D::SetPosition(const Vector3& position, TransformSpace space)
	{
		LocalPosition = TransformPosition(position, space, TransformSpace::Parent);
	}

	Vector3 Transform3D::GetPosition(TransformSpace space) const
	{
		return TransformPosition(LocalPosition, TransformSpace::Parent, space);
	}

	void Transform3D::SetRotation(const Quaternion& rotation, TransformSpace space)
	{
		LocalRotation = TransformRotation(rotation, space, TransformSpace::Parent);
	}

	Quaternion Transform3D::GetRotation(TransformSpace space) const
	{
		return TransformRotation(LocalRotation, TransformSpace::Parent, space);
	}

	void Transform3D::SetScale(const Vector3& scale, TransformSpace space)
	{
		LocalScale = TransformScale(scale, space, TransformSpace::Parent);
	}

	Vector3 Transform3D::GetScale(TransformSpace space) const
	{
		return TransformScale(LocalScale, TransformSpace::Parent, space);
	}

	void Transform3D::Decompose(TransformSpace space, Vector3& outPosition, Quaternion& outRotation, Vector3& outScale) const
	{
		Matrix4x4 m = GetTransformMatrix(TransformSpace::Self, space);
		m.Decompose(outPosition, outRotation, outScale);
	}

	void Transform3D::Transform(TransformSpace from, TransformSpace to, Vector3& position, Quaternion& rotation, Vector3& scale) const
	{
		position = TransformPosition(position, from, to);
		rotation = TransformRotation(rotation, from, to);
		scale = TransformScale(scale, from, to);
	}

	Vector3 Transform3D::GetForward(TransformSpace space) const
	{
		return GetTransformMatrix(TransformSpace::Self, space).GetForwardVector();
	}

	Vector3 Transform3D::GetBackward(TransformSpace space) const
	{
		return GetTransformMatrix(TransformSpace::Self, space).GetBackwardVector();
	}

	Vector3 Transform3D::GetRight(TransformSpace space) const
	{
		return GetTransformMatrix(TransformSpace::Self, space).GetRightVector();
	}

	Vector3 Transform3D::GetLeft(TransformSpace space) const
	{
		return GetTransformMatrix(TransformSpace::Self, space).GetLeftVector();
	}

	Vector3 Transform3D::GetUp(TransformSpace space) const
	{
		return GetTransformMatrix(TransformSpace::Self, space).GetUpVector();
	}

	Vector3 Transform3D::GetDown(TransformSpace space) const
	{
		return GetTransformMatrix(TransformSpace::Self, space).GetDownVector();
	}

	Matrix4x4 Transform3D::GetTransformMatrix(TransformSpace from, TransformSpace to) const
	{
		if (from == to)
		{
			return Matrix4x4::Identity;
		}

		switch (from)
		{
		case TransformSpace::Global:
		{
			switch (to)
			{
			case TransformSpace::Parent:
				return InvParentTransform;
			case TransformSpace::Self:
				return InvGlobalTransform;
			default:
				break;
			}
			break;
		}
		case TransformSpace::Parent:
		{
			switch (to)
			{
			case TransformSpace::Global:
				return ParentTransform;
			case TransformSpace::Self:
				return LocalTransform.Inverted();
			default:
				break;
			}
			break;
		}
		case TransformSpace::Self:
		{
			switch (to)
			{
			case TransformSpace::Global:
				return GlobalTransform;
			case TransformSpace::Parent:
				return LocalTransform;
			default:
				break;
			}
			break;
		}
		}

		Assert(false)
		return Matrix4x4::Identity;
	}
}