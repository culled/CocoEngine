#pragma once

#include "Graphics/Buffer.h"
#include "Graphics/GraphicsDevice.h"
#include "MeshTypes.h"

namespace Coco::Rendering
{
	struct SubMesh
	{
		uint32 Offset;
		uint32 Count;

		SubMesh();
		SubMesh(uint32 offset, uint32 count);
	};

	class Mesh
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
		bool _isDirty;

	public:
		Mesh();
		~Mesh();

		void SetVertices(const VertexDataFormat& format, std::span<VertexData> vertices);
		void SetIndices(std::span<uint32> indices, uint32 submeshIndex);

		bool Apply(bool deleteLocalData = true);

		const VertexDataFormat& GetVertexFormat() const { return _vertexFormat; }
		uint64 GetVertexCount() const { return _vertexCount; }
		uint64 GetIndexCount() const { return _indexCount; }

		Ref<Buffer> GetVertexBuffer() const { return _vertexBuffer; }
		Ref<Buffer> GetIndexBuffer() const { return _indexBuffer; }

		bool TryGetSubmesh(uint32 submeshID, SubMesh& outSubmesh) const;

	private:
		void MarkDirty();
	};
}