#pragma once

#include <Coco/Core/Resources/Resource.h>
#include "MeshTypes.h"
#include <Coco/Core/Types/BoundingBox.h>

namespace Coco::Rendering
{
	class Mesh :
		public Resource
	{
		friend class MeshSerializer;

	public:
		Mesh(const ResourceID& id, MeshUsageFlags usageFlags = MeshUsageFlags::None);

		// Inherited via Resource
		const std::type_info& GetType() const override { return typeid(Mesh); }
		const char* GetTypename() const override { return "Mesh"; }

		MeshUsageFlags GetUsageFlags() const { return _usageFlags; }

		void SetVertices(const VertexDataFormat& format, std::span<const VertexData> vertices);
		std::span<const VertexData> GetVertices() const { return _vertices; }
		uint64 GetVertexCount() const { return _vertexCount; }
		const VertexDataFormat& GetVertexFormat() const { return _vertexFormat; }

		void SetIndices(const std::unordered_map<uint32, std::vector<uint32>>& submeshIndices);
		void SetIndices(std::span<const uint32> indices, const std::unordered_map<uint32, Submesh>& submeshes);
		void SetIndices(std::span<const uint32> indices);
		uint64 GetIndexCount() const { return _indexCount; }
		bool TryGetSubmesh(uint32 id, const Submesh*& outSubmesh) const;
		const std::unordered_map<uint32, Submesh>& GetSubmeshes() const { return _submeshes; }

		void GetDataSizeRequirements(uint64& outVertexDataSize, uint64& outIndexDataSize) const;

		const BoundingBox& GetLocalBounds() const { return _localBounds; }

		bool NeedsUpload() const { return (_isDirty || (_usageFlags & MeshUsageFlags::Dynamic) == MeshUsageFlags::Dynamic) && (_vertexDataSize > 0 && _indexDataSize > 0); }
		bool UploadData(void* vertexBufferMemory, uint64 vertexMemorySize, void* indexBufferMemory, uint64 indexMemorySize);

	private:
		VertexDataFormat _vertexFormat;
		std::vector<VertexData> _vertices;
		uint64 _vertexDataSize;
		uint64 _vertexCount;

		std::vector<uint32> _indices;
		uint64 _indexDataSize;
		uint64 _indexCount;
		std::unordered_map<uint32, Submesh> _submeshes;

		MeshUsageFlags _usageFlags;
		bool _isDirty;

		BoundingBox _localBounds;

	private:
		void MarkDirty();
	};
}