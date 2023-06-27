#include "Mesh.h"

#include "RenderingService.h"
#include "Graphics/Resources/BufferTypes.h"

namespace Coco::Rendering
{
	Mesh::Mesh(ResourceID id, const string& name, uint64_t tickLifetime) : RenderingResource(id, name, tickLifetime)
	{}

	Mesh::~Mesh()
	{
		GraphicsPlatform* platform = EnsureRenderingService()->GetPlatform();
		platform->PurgeResource(_vertexBuffer);
		platform->PurgeResource(_indexBuffer);
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

		GraphicsPlatform* platform = EnsureRenderingService()->GetPlatform();

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

			_vertexCount = _vertexPositions.Count();
			_indexCount = _vertexIndices.Count();

			const uint64_t vertexBufferSize = _vertexCount * sizeof(VertexData);
			const uint64_t indexBufferSize = _indexCount * sizeof(uint32_t);

			if (!_vertexBuffer.IsValid())
				_vertexBuffer = platform->CreateBuffer(
					vertexBufferSize,
					BufferUsageFlags::TransferDestination | BufferUsageFlags::TransferSource | BufferUsageFlags::Vertex,
					true);

			if (!_indexBuffer.IsValid())
				_indexBuffer = platform->CreateBuffer(
					indexBufferSize,
					BufferUsageFlags::TransferDestination | BufferUsageFlags::TransferSource | BufferUsageFlags::Index,
					true);

			Ref<Buffer> stagingBuffer = platform->CreateBuffer(
				vertexBufferSize,
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

			// Resize for index data
			stagingBuffer->Resize(indexBufferSize, false);

			// Upload index data
			stagingBuffer->LoadData(0, _vertexIndices);
			stagingBuffer->CopyTo(0, _indexBuffer.Get(), 0, sizeof(uint32_t) * _vertexIndices.Count());

			platform->PurgeResource(stagingBuffer);

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
			LogError(platform->GetLogger(), FormattedString(
				"Failed to upload mesh data: {}",
				ex.what()
			));

			_vertexCount = 0;
			_indexCount = 0;

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
