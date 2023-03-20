#pragma once

#include "RenderingResource.h"

#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Vector.h>
#include "Graphics/Resources/Buffer.h"
#include "RenderingExceptions.h"

namespace Coco::Rendering
{
	/// <summary>
	/// Vertex data that can be sent to the GPU
	/// </summary>
	struct VertexData
	{
		float Position[3];
		float UV0[2];
	};

	/// <summary>
	/// Holds data for rendering geometry
	/// </summary>
	class COCOAPI Mesh : public RenderingResource
	{
	public:
		/// <summary>
		/// The size of the vertex buffer (in bytes)
		/// </summary>
		static const uint64_t VertexBufferSize;

		/// <summary>
		/// The size of the index buffer (in bytes)
		/// </summary>
		static const uint64_t IndexBufferSize;

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
		Mesh();
		virtual ~Mesh();

		/// <summary>
		/// Sets vertex positions for this mesh.
		/// NOTE: this will define the number of vertices this mesh has
		/// </summary>
		/// <param name="positions">A list of vertex positions</param>
		void SetPositions(const List<Vector3>& positions);

		/// <summary>
		/// Sets vertex positions for this mesh.
		/// NOTE: this will define the number of vertices this mesh has
		/// </summary>
		/// <param name="positions">A list of vertex positions</param>
		void SetUVs(const List<Vector2>& uvs);

		/// <summary>
		/// Sets the indices for this mesh.
		/// NOTE: must be a multiple of [vertex count * 3]
		/// </summary>
		/// <param name="indices">The list of vertex indices</param>
		void SetIndices(const List<uint32_t>& indices);

		/// <summary>
		/// Gets if this mesh has changes that haven't been uploaded to the GPU
		/// </summary>
		/// <returns>True if this mesh has changes that haven't been uploaded to the GPU</returns>
		bool GetIsDirty() const noexcept { return _isDirty; }

		/// <summary>
		/// Uploads this mesh's data to the GPU
		/// </summary>
		/// <param name="deleteLocalData">If true, the local mesh data will be cleared and will solely live on the GPU</param>
		/// <returns>True if the data was uploaded successfully</returns>
		bool UploadData(bool deleteLocalData = true);

		/// <summary>
		/// Gets this mesh's vertex buffer
		/// </summary>
		/// <returns>The vertex buffer</returns>
		WeakManagedRef<Buffer> GetVertexBuffer() const noexcept { return _vertexBuffer; }

		/// <summary>
		/// Gets this mesh's index buffer
		/// </summary>
		/// <returns>The index buffer</returns>
		WeakManagedRef<Buffer> GetIndexBuffer() const noexcept { return _indexBuffer; }

		/// <summary>
		/// Gets the number of vertices in this mesh
		/// </summary>
		/// <returns>The number of vertices</returns>
		uint64_t GetVertexCount() const noexcept { return _vertexCount; }

		/// <summary>
		/// Gets the number of indices in the mesh
		/// </summary>
		/// <returns>The number of vertex indices</returns>
		uint64_t GetIndexCount() const noexcept { return _indexCount; }

	private:
		/// <summary>
		/// Marks this mesh as dirty and needing a re-upload of data
		/// </summary>
		void MarkDirty() noexcept;
	};
}