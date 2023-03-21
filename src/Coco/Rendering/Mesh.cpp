#include "Mesh.h"

#include "RenderingService.h"
#include "Graphics/Resources/BufferTypes.h"

namespace Coco::Rendering
{
	const uint64_t Mesh::VertexBufferSize = sizeof(VertexData) * 1024 * 1024;
	const uint64_t Mesh::IndexBufferSize = sizeof(uint32_t) * 1024 * 1024;

	Mesh::Mesh() : RenderingResource(ResourceType::Mesh)
	{
		GraphicsPlatform* platform = EnsureRenderingService()->GetPlatform();

		_vertexBuffer = platform->CreateBuffer(
			VertexBufferSize, 
			BufferUsageFlags::TransferDestination | BufferUsageFlags::TransferSource | BufferUsageFlags::Vertex,
			true);

		_indexBuffer = platform->CreateBuffer(
			IndexBufferSize, 
			BufferUsageFlags::TransferDestination | BufferUsageFlags::TransferSource | BufferUsageFlags::Index, 
			true);
	}

	Mesh::~Mesh()
	{
		_vertexBuffer.Invalidate();
		_indexBuffer.Invalidate();
	}

	void Mesh::SetPositions(const List<Vector3>& positions)
	{
		_vertexPositions = positions;
		MarkDirty();
	}

	void Mesh::SetUVs(const List<Vector2>& uvs)
	{
		_vertexUV0s = uvs;
		MarkDirty();
	}

	void Mesh::SetIndices(const List<uint32_t>& indices)
	{
		_vertexIndices = indices;
		MarkDirty();
	}

	bool Mesh::UploadData(bool deleteLocalData)
	{
		if (!_isDirty)
			return true;

		RenderingService* renderingService = EnsureRenderingService();

		try
		{
			if (_vertexUV0s.Count() != _vertexPositions.Count())
			{
				throw InvalidOperationException(FormattedString(
					"UV0 count ({}) doesn't match vertex count of {}",
					_vertexUV0s.Count(),
					_vertexPositions.Count()));
			}

			if (_vertexIndices.Count() % 3 != 0)
			{
				throw InvalidOperationException(FormattedString(
					"Index count must be a multiple of 3: {} % 3 != 0",
					_vertexIndices.Count()));
			}

			if (!_vertexBuffer.IsValid())
				throw InvalidOperationException("Lost vertex buffer resource");

			if (!_indexBuffer.IsValid())
				throw InvalidOperationException("Lost index buffer resource");

			WeakManagedRef<Buffer> stagingBuffer = renderingService->GetPlatform()->CreateBuffer(
				VertexBufferSize,
				BufferUsageFlags::TransferSource | BufferUsageFlags::TransferDestination | BufferUsageFlags::HostVisible,
				true);

			// Build the vertex data
			List<VertexData> vertexData(_vertexPositions.Count());
			for (uint64_t i = 0; i < vertexData.Count(); i++)
			{
				vertexData[i].Position[0] = static_cast<float>(_vertexPositions[i].X);
				vertexData[i].Position[1] = static_cast<float>(_vertexPositions[i].Y);
				vertexData[i].Position[2] = static_cast<float>(_vertexPositions[i].Z);

				vertexData[i].UV0[0] = static_cast<float>(_vertexUV0s[i].X);
				vertexData[i].UV0[1] = static_cast<float>(_vertexUV0s[i].Y);
			}

			// Upload vertex data
			stagingBuffer->LoadData(0, vertexData);
			stagingBuffer->CopyTo(0, _vertexBuffer.Get(), 0, sizeof(VertexData) * vertexData.Count());
			_vertexCount = vertexData.Count();

			// Resize for index data
			stagingBuffer->Resize(IndexBufferSize, false);

			// Upload index data
			stagingBuffer->LoadData(0, _vertexIndices);
			stagingBuffer->CopyTo(0, _indexBuffer.Get(), 0, sizeof(uint32_t) * _vertexIndices.Count());
			_indexCount = _vertexIndices.Count();

			stagingBuffer.Invalidate();

			if (deleteLocalData)
			{
				_vertexPositions.Clear();
				_vertexUV0s.Clear();
				_vertexIndices.Clear();
			}

			_isDirty = false;

			return true;
		}
		catch (const Exception& ex)
		{
			LogError(renderingService->GetLogger(), FormattedString(
				"Failed to upload mesh data: {}",
				ex.what()
			));

			return false;
		}
	}

	void Mesh::MarkDirty() noexcept
	{
		if (_isDirty)
			return;

		IncrementVersion();
		_isDirty = true;
	}
}
