#pragma once

#include "../EntityComponent.h"

#include <Coco/Core/Types/Rect.h>
#include <Coco/Core/Types/Quaternion.h>
#include <Coco/Core/Types/Matrix.h>

namespace Coco::ECS
{
	/// @brief Defines 2D transform data for an entity
	class COCOAPI RectTransformComponent : public EntityComponent
	{
	private:
		Vector2 _localPosition;
		double _localRotation;
		Size _localSize;
		Vector2 _localScale;
		Vector2 _pivot;
		int _localZIndex;

		bool _isLocalTransformMatrixDirty = false;
		Matrix4x4 _localTransformMatrix = Matrix4x4::Identity;
		
		bool _isGlobalTransformMatrixDirty = false;
		Matrix4x4 _globalTransformMatrix = Matrix4x4::Identity;
		Matrix4x4 _invGlobalTransformMatrix = Matrix4x4::Identity;

		bool _inheritParentTransform = true;

	public:
		RectTransformComponent() = default;
		RectTransformComponent(const EntityID& owner);
		RectTransformComponent(const EntityID& owner, const Vector2& position, double rotation, const Size& size, const Vector2& pivot = Vector2::Zero, int zIndex = 0);

		virtual ~RectTransformComponent() = default;

		/// @brief Gets the transform matrix local to the parent
		/// @return The local transform matrix
		const Matrix4x4& GetLocalTransformMatrix();

		/// @brief Gets the global transform matrix
		/// @param sized If true, then the matrix's scale component will also include this transform's size
		/// @return The global transform matrix
		Matrix4x4 GetGlobalTransformMatrix(bool sized = false);

		/// @brief Gets the inverse of the global transform matrix
		/// @return The inverse of the global transform matrix
		const Matrix4x4& GetInverseGlobalTransformMatrix();

		/// @brief Sets if this transform should inherit its parent's transform
		/// @param inheritParentTransform If true, this entity's transform will move with its parent's
		void SetInheritParentTransform(bool inheritParentTransform);

		/// @brief Gets if this transform inherits and moves with its parent's transform
		/// @return If this transform inherits and moves with its parent's transform
		bool GetInheritParentTransform() const { return _inheritParentTransform; }

		/// @brief Sets the position local to the parent
		/// @param position The local position
		void SetLocalPosition(const Vector2& position);

		/// @brief Gets the position local to the parent
		/// @return The local position
		const Vector2& GetLocalPosition() const { return _localPosition; }

		/// @brief Sets the rotation local to the parent
		/// @param rotation The local rotation (in radians)
		void SetLocalRotation(double rotation);

		/// @brief Gets the rotation local to the parent
		/// @return The local rotation
		double GetLocalRotation() const { return _localRotation; }

		/// @brief Sets the pivot point of this transform.
		/// A pivot of (0, 0) is the bottom-left corner, and a pivot of (1, 1) is the top-right corner
		/// @param pivot The pivot point
		void SetPivot(const Vector2& pivot);

		/// @brief Gets the pivot point
		/// @return The pivot point
		const Vector2& GetPivot() const { return _pivot; }

		/// @brief Sets the size local to the parent
		/// @param size The local size
		void SetLocalSize(const Size& size);

		/// @brief Gets the size local to the parent
		/// @return The local size
		const Size& GetLocalSize() const { return _localSize; }

		/// @brief Sets the z index local to the parent. Transforms with higher z indices are drawn above those with lower indices
		/// @param zIndex The local z index
		void SetLocalZIndex(int zIndex);

		/// @brief Gets the z index local to the parent
		/// @return The local z index
		int GetLocalZIndex() const { return _localZIndex; }

		/// @brief Sets the scale local to the parent
		/// @param scale The local scale
		void SetLocalScale(const Vector2& scale);

		/// @brief Gets the scale local to the parent
		/// @return The local scale
		const Vector2& GetLocalScale() const { return _localScale; }

		/// @brief Gets the parent transform, checking first if it exists
		/// @param parentTransform Will be set to the parent transform
		/// @return True if the parent transform exists
		bool TryGetParent(RectTransformComponent*& parentTransform);

	private:
		/// @brief Marks this transform as needing to be recalculated
		void InvalidateTransform();

		/// @brief Updates the local transform matrix
		void UpdateLocalTransform();

		/// @brief Updates the global transform matrix
		void UpdateGlobalTransform();
	};
}