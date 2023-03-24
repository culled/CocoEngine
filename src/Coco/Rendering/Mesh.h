#pragma once

#include "RenderingResource.h"

#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Vector.h>
#include "Graphics/Resources/Buffer.h"
#include "RenderingExceptions.h"

namespace Coco::Rendering
{
	/// @brief Vertex data that can be sent to the GPU
	struct VertexData
	{
		float Position[3];
		float UV0[2];
	};

	/// @brief Holds vertex and index data for rendering geometry
	class COCOAPI Mesh : public RenderingResource
	{
	public:
		/// @brief The size of the vertex buffer (in bytes)
		static constexpr uint64_t VertexBufferSize = sizeof(VertexData) * 1024 * 1024;

		/// @brief The size of the index buffer (in bytes)
		static constexpr uint64_t IndexBufferSize = sizeof(uint32_t) * VertexBufferSize * 3;

	private:
		WeakManagedRef<Buffer> _vertexBuffer;
		WeakManagedRef<Buffer> _indexBuffer;

		List<Vector3> _vertexPositions;
		List<Vector2> _vertexUV0s;
		List<uint32_t> _vertexIndices;

		uint64_t _vertexCount = 0;
		uint64_t _indexCount = 0;

		bool _isDirty = false;

	public:
		Mesh(const string& name = "");
		virtual ~Mesh();

		/// @brief Sets vertex positions for this mesh. NOTE: this will define the number of vertices this mesh has
		/// @param positions The list of vertex positions
		void SetPositions(const List<Vector3>& positions);

		/// @brief Sets the uvs for the mesh
		/// @param uvs The list of UV coordinates
		void SetUVs(const List<Vector2>& uvs);

		/// @brief Sets the indices for this mesh. NOTE: must be a multiple of <vertex count * 3>
		/// @param indices The list of vertex indices
		void SetIndices(const List<uint32_t>& indices);

		/// @brief Gets if this mesh has changes that haven't been uploaded to the GPU
		/// @return True if this mesh has changes that haven't been uploaded to the GPU
		bool GetIsDirty() const noexcept { return _isDirty; }

		/// @brief Uploads any pending changes of this mesh's data to the GPU
		/// @param deleteLocalData If true, the local mesh data will be cleared and will solely live on the GPU
		/// @return True if the data was uploaded successfully
		bool UploadData(bool deleteLocalData = true);

		/// @brief Gets this mesh's vertex buffer
		/// @return The vertex buffer
		WeakManagedRef<Buffer> GetVertexBuffer() const noexcept { return _vertexBuffer; }

		/// @brief Gets this mesh's index buffer
		/// @return The index buffer
		WeakManagedRef<Buffer> GetIndexBuffer() const noexcept { return _indexBuffer; }

		/// @brief Gets the number of vertices in this mesh
		/// @return The number of vertices
		constexpr uint64_t GetVertexCount() const noexcept { return _vertexCount; }

		/// @brief Gets the number of indices in the mesh
		/// @return The number of vertex indices
		constexpr uint64_t GetIndexCount() const noexcept { return _indexCount; }

	private:
		/// @brief Marks this mesh as dirty and needing a re-upload of data
		void MarkDirty() noexcept;
	};
}