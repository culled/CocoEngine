#pragma once

#include "RendererResource.h"
#include <Coco/Core/Types/Refs.h>
#include "MeshTypes.h"
#include "Graphics/Buffer.h"
#include <Coco/Core/Types/BoundingBox.h>

namespace Coco::Rendering
{
	/// @brief A region in a mesh's index buffer
	struct SubMesh
	{
		/// @brief The offset of the first index in the buffer
		uint64 Offset;

		/// @brief The number of indices in the buffer
		uint64 Count;

		/// @brief The bounds that this submesh fits into
		BoundingBox Bounds;

		SubMesh();
		SubMesh(uint64 offset, uint64 count, const BoundingBox& bounds);
	};

	/// @brief Defines geometry data used for rendering
	class Mesh : public RendererResource
	{
	private:
		Ref<Buffer> _vertexBuffer;
		Ref<Buffer> _indexBuffer;
		VertexDataFormat _vertexFormat;
		std::vector<VertexData> _vertices;
		std::unordered_map<uint32, std::vector<uint32>> _indices;
		std::unordered_map<uint32, SubMesh> _submeshes;
		uint64 _vertexCount;
		uint64 _indexCount;
		bool _isDynamic;
		bool _isDirty;
		void* _lockedVertexMemory;
		void* _lockedIndexMemory;
		BoundingBox _meshBounds;

	public:
		Mesh(const ResourceID& id, const string& name, bool isDynamic = false);
		~Mesh();

		std::type_index GetType() const final { return typeid(Mesh); }

		/// @brief Sets the vertices of this mesh
		/// @param format The format of the vertex data
		/// @param vertices The vertices
		void SetVertices(const VertexDataFormat& format, std::span<const VertexData> vertices);

		/// @brief Sets the indices of this mesh
		/// @param indices The indices
		/// @param submeshID The id of the submesh that these indices reference
		void SetIndices(std::span<const uint32> indices, uint32 submeshID);

		/// @brief Uploads any pending changes to the GPU
		/// @param deleteLocalData If true, vertex and index data will be deleted locally and will be soley stored on the GPU
		/// @return True if the apply was successful
		bool Apply(bool deleteLocalData = true);

		/// @brief Gets the format of this mesh's vertex data
		/// @return The format of the vertex data
		const VertexDataFormat& GetVertexFormat() const { return _vertexFormat; }

		/// @brief Gets the number of vertices this mesh has
		/// @return The number of vertices
		uint64 GetVertexCount() const { return _vertexCount; }

		/// @brief Gets the number of indices this mesh has
		/// @return The number of indices
		uint64 GetIndexCount() const { return _indexCount; }

		/// @brief Gets this mesh's vertex buffer
		/// @return The vertex buffer
		Ref<Buffer> GetVertexBuffer() const { return _vertexBuffer; }

		/// @brief Gets this mesh's index buffer
		/// @return The index buffer
		Ref<Buffer> GetIndexBuffer() const { return _indexBuffer; }

		/// @brief Gets the bounds that this entire mesh fits in
		/// @return The bounds of this mesh
		const BoundingBox& GetBounds() const { return _meshBounds; }

		/// @brief Clears all submeshes and their index data from this mesh
		void ClearSubmeshes();

		/// @brief Gets data for a given submesh.
		/// NOTE: this is only valid after this mesh has been applied
		/// @param submeshID The ID of the submesh
		/// @param outSubmesh Will be set the the submesh data if found
		/// @return True if a submesh with the given ID was found
		bool TryGetSubmesh(uint32 submeshID, SubMesh& outSubmesh) const;

		/// @brief Gets all submeshes.
		/// NOTE: this is only valid after this mesh has been applied
		/// @return All submeshes and their ids
		const std::unordered_map<uint32, SubMesh>& GetSubmeshes() const { return _submeshes; }

		/// @brief Calculates this mesh's submeshes
		void CalculateSubmeshes();

	private:
		/// @brief Calculates the bounds of a submesh
		/// @param submeshID The ID of the submesh
		/// @return The bounds of the submesh
		BoundingBox CalculateSubmeshBounds(uint32 submeshID);

		/// @brief Marks this mesh as needing updates
		void MarkDirty();
	};
}