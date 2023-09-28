#pragma once

#include "../Corepch.h"
#include "Vector.h"

namespace Coco
{
	struct Matrix4x4;

	/// @brief A 3D axis-aligned bounding box
	struct BoundingBox
	{
		/// @brief A bounding box with zero size at the world origin
		static const BoundingBox Zero;

		/// @brief The minimum point
		Vector3 Minimum;

		/// @brief The maximum point
		Vector3 Maximum;

		BoundingBox();
		BoundingBox(const Vector3& p0, const Vector3& p1);

		/// @brief Creates a bounding box with a center point and a size 
		/// @param center The center of the box
		/// @param size The size on each axis
		/// @return The bounding box
		static BoundingBox FromCenterAndSize(const Vector3& center, const Vector3& size);

		/// @brief Creates a bounding box encompassing the given list of points
		/// @param points The points
		/// @return The bounding box
		static BoundingBox FromPoints(std::span<const Vector3> points);

		/// @brief Gets the minimum corner of this box
		/// @return The minimum corner
		const Vector3& GetMinimum() const { return Minimum; }

		/// @brief Gets the maximum corner of this box
		/// @return The maximum corner
		const Vector3& GetMaximum() const { return Maximum; }

		/// @brief Gets the center of this box
		/// @return The center
		Vector3 GetCenter() const;

		/// @brief Gets the size on each axis of this box
		/// @return The size
		Vector3 GetSize() const;

		/// @brief Calculates the volume of this box
		/// @return The volume
		double GetVolume() const;

		/// @brief Gets the left side (-X) of this box
		/// @return The left side position
		constexpr double GetLeft() const { return Minimum.X; }

		/// @brief Gets the right side (+X) of this box
		/// @return The right side position
		constexpr double GetRight() const { return Maximum.X; }

		/// @brief Gets the bottom side (-Y) of this box
		/// @return The bottom side position
		constexpr double GetBottom() const { return Minimum.Y; }

		/// @brief Gets the top side (+Y) of this box
		/// @return The top side position
		constexpr double GetTop() const { return Maximum.Y; }

		/// @brief Gets the front side (-Z) of this box
		/// @return The front side position
		constexpr double GetFront() const { return Minimum.Z; }

		/// @brief Gets the back side (+Z) of this box
		/// @return The back side position
		constexpr double GetBack() const { return Maximum.Z; }

		/// @brief Moves this bounding box
		/// @param translation The amount to move
		void Translate(const Vector3& translation);

		/// @brief Expands this box to encompass the given point
		/// @param point The point
		void Expand(const Vector3& point);

		/// @brief Expands this box to encompass the given box
		/// @param box The box
		void Expand(const BoundingBox& box);

		/// @brief Inflates this box on each axis by the given amount
		/// @param amount The amount to change the size on each axis
		void Inflate(const Vector3& amount);

		/// @brief Checks if a point intersects this box
		/// @param point The point
		/// @return True if the point intersects this box
		bool Intersects(const Vector3& point);

		/// @brief Checks if another bounding box intersects this one
		/// @param other The other bounding box
		/// @return 
		bool Intersects(const BoundingBox& other);

		/// @brief Computes a bounding box that fits around this box if it were transformed by the given matrix
		/// @param trsMatrix The transformation matrix
		/// @return The transformed bounding box
		BoundingBox Transformed(const Matrix4x4& trsMatrix) const;

		/// @brief Gets points that represent the 8 corners of this bounding box
		/// @return The corner points
		std::array<Vector3, 8> GetCorners() const;
	};
}