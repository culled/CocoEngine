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
		CalculateLocalMatrix();
		CalculateGlobalMatrix(parent);
	}

	void Transform2D::CalculateLocalMatrix()
	{
		LocalTransform = Matrix4x4::CreateTransform(Vector3(LocalPosition), Quaternion(Vector3(0.0, 0.0, LocalRotation)), Vector3(LocalScale, 1.0));
	}

	void Transform2D::CalculateGlobalMatrix(const Transform2D* parent)
	{
		if (parent)
		{
			GlobalTransform = LocalTransform * parent->GlobalTransform;
		}
		else
		{
			GlobalTransform = LocalTransform;
		}

		InvGlobalTransform = GlobalTransform.Inverted();
	}

	void Transform2D::Recalculate(const Transform2D* parent)
	{
		CalculateLocalMatrix();
		CalculateGlobalMatrix(parent);
	}

	Vector2 Transform2D::GlobalToLocalPosition(const Vector2& position) const
	{
		Vector4 v = InvGlobalTransform * Vector4(position, 0.0, 1.0);
		return Vector2(v.X, v.Y);
	}

	double Transform2D::GlobalToLocalRotation(double rotation) const
	{
		Vector3 e = (InvGlobalTransform.GetRotation() * Quaternion(Vector3(0.0, 0.0, rotation))).ToEulerAngles();
		return e.Z;
	}

	Vector2 Transform2D::GlobalToLocalScale(const Vector2& scale) const
	{
		Quaternion rotation = Quaternion(Vector3(0.0, 0.0, GetGlobalRotation()));
		Vector3 rotated = rotation * Vector3(scale, 0.0);

		return GlobalToLocalVector(rotated.XY());
	}

	Vector2 Transform2D::GlobalToLocalVector(const Vector2& vector) const
	{
		Vector4 v = InvGlobalTransform * Vector4(vector, 0.0, 0.0);
		return Vector2(v.X, v.Y);
	}

	Vector2 Transform2D::LocalToGlobalPosition(const Vector2& position) const
	{
		Vector4 v = GlobalTransform * Vector4(position, 0.0, 1.0);
		return Vector2(v.X, v.Y);
	}

	double Transform2D::LocalToGlobalRotation(double rotation) const
	{
		Vector3 e = (GlobalTransform.GetRotation() * Quaternion(Vector3(0.0, 0.0, rotation))).ToEulerAngles();
		return e.Z;
	}

	Vector2 Transform2D::LocalToGlobalScale(const Vector2& scale) const
	{
		Vector3 s = Vector3(LocalToGlobalVector(scale), 0.0);
		Quaternion invRotation = Quaternion(Vector3(0.0, 0.0, GetGlobalRotation())).Inverted();
		return (invRotation * s).XY();
	}

	Vector2 Transform2D::LocalToGlobalVector(const Vector2& vector) const
	{
		Vector4 v = GlobalTransform * Vector4(vector, 0.0, 0.0);
		return Vector2(v.X, v.Y);
	}

	void Transform2D::TranslateLocal(const Vector2& translation)
	{
		LocalPosition += translation;
	}

	void Transform2D::TranslateGlobal(const Vector2& translation, const Transform2D* parent)
	{
		if (parent)
		{
			TranslateLocal(parent->GlobalToLocalVector(translation));
		}
		else
		{
			TranslateLocal(translation);
		}
	}

	void Transform2D::RotateLocal(double rotation)
	{
		LocalRotation += rotation;
	}

	void Transform2D::RotateGlobal(double rotation, const Transform2D* parent)
	{
		if (parent)
		{
			RotateLocal(parent->GlobalToLocalRotation(rotation));
		}
		else
		{
			RotateLocal(rotation);
		}
	}

	void Transform2D::GetGlobalTransform(Vector2& outPosition, double& outRotation, Vector2& outScale) const
	{
		Vector3 p, s;
		Quaternion r;

		GlobalTransform.Decompose(p, r, s);

		outPosition = p.XY();
		outRotation = r.ToEulerAngles().Z;
		outScale = s.XY();
	}

	void Transform2D::TransformGlobalToLocal(Vector2& position, double& rotation, Vector2& scale) const
	{
		position = GlobalToLocalPosition(position);
		rotation = GlobalToLocalRotation(rotation);
		scale = GlobalToLocalScale(scale);
	}

	Transform3D::Transform3D() : 
		Transform3D(Vector3::Zero, Quaternion::Identity, Vector3::One)
	{}

	Transform3D::Transform3D(const Vector3& position, const Quaternion& rotation, const Vector3& scale, const Transform3D* parent) :
		LocalPosition(position),
		LocalRotation(rotation),
		LocalScale(scale),
		LocalTransform(),
		GlobalTransform(),
		InvGlobalTransform()
	{
		Recalculate(parent);
	}

	void Transform3D::CalculateLocalMatrix()
	{
		LocalTransform = Matrix4x4::CreateTransform(LocalPosition, LocalRotation, LocalScale);
	}

	void Transform3D::CalculateGlobalMatrix(const Transform3D* parent)
	{
		if (parent)
		{
			GlobalTransform = LocalTransform * parent->GlobalTransform;
		}
		else
		{
			GlobalTransform = LocalTransform;
		}

		InvGlobalTransform = GlobalTransform.Inverted();
	}

	void Transform3D::Recalculate(const Transform3D* parent)
	{
		CalculateLocalMatrix();
		CalculateGlobalMatrix(parent);
	}

	Vector3 Transform3D::GlobalToLocalPosition(const Vector3& position) const
	{
		Vector4 v = InvGlobalTransform * Vector4(position, 1.0);
		return Vector3(v.X, v.Y, v.Z);
	}

	Quaternion Transform3D::GlobalToLocalRotation(const Quaternion& rotation) const
	{
		return InvGlobalTransform.GetRotation() * rotation;
	}

	Vector3 Transform3D::GlobalToLocalVector(const Vector3& vector) const
	{
		Vector4 v = InvGlobalTransform * Vector4(vector, 0.0);
		return Vector3(v.X, v.Y, v.Z);
	}

	Vector3 Transform3D::GlobalToLocalScale(const Vector3& scale) const
	{
		Vector3 s = GetGlobalRotation() * scale;

		return GlobalToLocalVector(s);
	}

	Vector3 Transform3D::LocalToGlobalPosition(const Vector3& position) const
	{
		Vector4 v = GlobalTransform * Vector4(position, 1.0);
		return Vector3(v.X, v.Y, v.Z);
	}

	Quaternion Transform3D::LocalToGlobalRotation(const Quaternion& rotation) const
	{
		return rotation * GlobalTransform.GetRotation();
	}

	Vector3 Transform3D::LocalToGlobalVector(const Vector3& vector) const
	{
		Vector4 v = GlobalTransform * Vector4(vector, 0.0);
		return Vector3(v.X, v.Y, v.Z);
	}

	Vector3 Transform3D::LocalToGlobalScale(const Vector3& scale) const
	{
		Vector3 s = LocalToGlobalVector(scale);
		Quaternion invRotation = GetGlobalRotation().Inverted();
		return invRotation * s;
	}

	void Transform3D::TranslateLocal(const Vector3& translation)
	{
		LocalPosition += translation;
	}

	void Transform3D::TranslateGlobal(const Vector3& translation, const Transform3D* parent)
	{
		if (parent)
		{
			TranslateLocal(parent->GlobalToLocalVector(translation));
		}
		else
		{
			TranslateLocal(translation);
		}
	}

	void Transform3D::RotateLocal(const Quaternion& rotation)
	{
		LocalRotation *= rotation;
	}

	void Transform3D::RotateLocal(const Vector3& axis, double angleRadians)
	{
		LocalRotation *= Quaternion(axis, angleRadians);
	}

	void Transform3D::RotateGlobal(const Quaternion& rotation, const Transform3D* parent)
	{
		if (parent)
		{
			RotateLocal(parent->GlobalToLocalRotation(rotation));
		}
		else
		{
			RotateLocal(rotation);		
		}
	}

	void Transform3D::RotateGlobal(const Vector3& axis, double angleRadians, const Transform3D* parent)
	{
		RotateGlobal(Quaternion(axis, angleRadians), parent);
	}

	void Transform3D::GetGlobalTransform(Vector3& outPosition, Quaternion& outRotation, Vector3& outScale) const
	{
		GlobalTransform.Decompose(outPosition, outRotation, outScale);
	}

	void Transform3D::TransformGlobalToLocal(Vector3& position, Quaternion& rotation, Vector3& scale) const
	{
		position = GlobalToLocalPosition(position);
		rotation = GlobalToLocalRotation(rotation);
		scale = GlobalToLocalScale(scale);
	}
}