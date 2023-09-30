#pragma once

#include "Renderpch.h"
#include <Coco/Core/Defines.h>
#include <Coco/Core/Types/Vector.h>

namespace Coco::Rendering
{
	/// @brief Defines a format for vertex data
	struct VertexDataFormat
	{
		/// @brief If true, the vertex data includes normal data
		bool HasNormals;

		/// @brief If true, the vertex data includes color data
		bool HasColor;

		/// @brief If true, the vertex data includes tangent data
		bool HasTangents;

		/// @brief If true, the vertex data includes uv0 data
		bool HasUV0;

		VertexDataFormat();

		/// @brief Gets the size of this format's vertex data, in bytes
		/// @return The size of the vertex data
		uint16 GetVertexSize() const;
	};

	/// @brief Defines a single vertex
	struct VertexData
	{
		/// @brief The maximum size of a single vertex's data, in bytes
		static const uint16 MaxVertexSize;

		/// @brief The vertex position
		Vector3 Position;

		/// @brief The vertex normal
		Vector3 Normal;

		/// @brief The vertex color
		Vector4 Color;

		/// @brief The vertex tangent
		Vector4 Tangent;

		/// @brief The first texture coordinate
		Vector2 UV0;

		VertexData();
		VertexData(const Vector3& position);
	};

	/// @brief Converts the given vertex data into a buffer-friendly format
	/// @param format The vertex format
	/// @param data The vertex data
	/// @return The buffer-friendly vertex data
	std::vector<uint8> GetVertexData(const VertexDataFormat& format, std::span<VertexData> data);
}