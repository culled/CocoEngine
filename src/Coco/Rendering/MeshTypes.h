#pragma once

#include "Renderpch.h"
#include <Coco/Core/Defines.h>
#include <Coco/Core/Types/Vector.h>
#include <Coco/Core/Types/BoundingBox.h>
#include "Graphics/BufferTypes.h"

namespace Coco::Rendering
{
	/// @brief Additional attributes for vertex data
	enum class VertexAttrFlags
	{
		None = 0,
		Normal = 1 << 0,
		Color = 1 << 1,
		Tangent = 1 << 2,
		UV0 = 1 << 3,
		All = Normal | Color | Tangent | UV0
	};

	DefineFlagOperators(VertexAttrFlags)

	/// @brief Flags to define the usage of a mesh
	enum class MeshUsageFlags
	{
		None = 0,
		Dynamic = 1 << 0,
		RetainCPUData = 1 << 1
	};

	DefineFlagOperators(MeshUsageFlags)

	/// @brief Defines a format for vertex data
	struct VertexDataFormat
	{
		/// @brief Additional attributes for a vertex
		VertexAttrFlags AdditionalAttributes;

		VertexDataFormat();
		VertexDataFormat(VertexAttrFlags additionalAttributes);

		bool operator==(const VertexDataFormat& other) const { return AdditionalAttributes == other.AdditionalAttributes; }

		/// @brief Gets the size of this format's vertex data, in bytes
		/// @param vertexCount The number of vertices
		/// @return The size of the vertex data
		uint64 GetDataSize(uint64 vertexCount) const;

		/// @brief Gets the stride of the additional attribute data, in bytes
		/// @return The additional attribute data stride
		uint64 GetAdditionalAttrStride() const;

		/// @brief Calls a callback for each additional vertex attribute flag in this format
		/// @param callback The callback function that will be called for each attribute
		void ForEachAdditionalAttr(std::function<void(VertexAttrFlags, BufferDataType)> callback) const;
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

	/// @brief Defines a series of indices to render mesh data
	struct Submesh
	{
		/// @brief The offset of the first index in the index buffer
		uint64 IndexOffset;

		/// @brief The number of indices
		uint64 IndexCount;

		Submesh();
		Submesh(uint64 indexOffset, uint64 indexCount);
	};

	std::vector<uint8> GetVertexBufferData(const VertexDataFormat& format, std::span<const VertexData> vertices);

	BoundingBox CalculateBounds(std::span<const VertexData> vertices);
	BoundingBox CalculateBounds(std::span<const VertexData> vertices, std::span<const uint32> indices);
}