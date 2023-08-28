#include "Mesh.h"

#include "RenderingService.h"
#include "Graphics/Resources/BufferTypes.h"

namespace Coco::Rendering
{
	Mesh::Mesh(const ResourceID& id, const string& name) : RenderingResource(id, name)
	{}

	Mesh::~Mesh()
	{
		GraphicsPlatform* platform = EnsureRenderingService()->GetPlatform();

		if(_vertexBuffer.IsValid())
			platform->PurgeResource(_vertexBuffer);

		if(_indexBuffer.IsValid())
			platform->PurgeResource(_indexBuffer);
	}

	void Mesh::SetPositions(const List<Vector3>& positions)
	{
		_vertexPositions = positions;
		MarkDirty();
	}

	void Mesh::SetNormals(const List<Vector3>& normals)
	{
		_vertexNormals = normals;
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
			if (_vertexIndices.Count() % 3 != 0)
			{
				throw InvalidOperationException(FormattedString(
					"Index count must be a multiple of 3: {} % 3 != 0",
					_vertexIndices.Count()));
			}

			_vertexCount = _vertexPositions.Count();
			_indexCount = _vertexIndices.Count();

			bool hasNormals = _vertexNormals.Count() > 0;
			bool hasUV0s = _vertexUV0s.Count() > 0;

			uint64_t vertexStride = sizeof(float) * 3;

			if (hasNormals)
				vertexStride += sizeof(float) * 3;

			if (hasUV0s)
				vertexStride += sizeof(float) * 2;

			const uint64_t vertexBufferSize = _vertexCount * vertexStride;
			const uint64_t indexBufferSize = _indexCount * sizeof(uint32_t);

			if (!_vertexBuffer.IsValid())
				_vertexBuffer = platform->CreateBuffer(FormattedString("{} vertex buffer", _name),
					vertexBufferSize,
					BufferUsageFlags::TransferDestination | BufferUsageFlags::TransferSource | BufferUsageFlags::Vertex,
					true);

			if (!_indexBuffer.IsValid())
				_indexBuffer = platform->CreateBuffer(FormattedString("{} index buffer", _name),
					indexBufferSize,
					BufferUsageFlags::TransferDestination | BufferUsageFlags::TransferSource | BufferUsageFlags::Index,
					true);

			Ref<Buffer> stagingBuffer = platform->CreateBuffer(FormattedString("{} staging buffer", _name),
				vertexBufferSize,
				BufferUsageFlags::TransferSource | BufferUsageFlags::TransferDestination | BufferUsageFlags::HostVisible,
				true);

			Array<float, 4> tempData = { 0.0f };

			// Build the vertex data
			List<char> vertexData(vertexBufferSize);
			for (uint64_t i = 0; i < _vertexCount; i++)
			{
				uint64_t offset = i * vertexStride;
				char* dst = vertexData.Data() + offset;

				tempData[0] = static_cast<float>(_vertexPositions[i].X);
				tempData[1] = static_cast<float>(_vertexPositions[i].Y);
				tempData[2] = static_cast<float>(_vertexPositions[i].Z);

				std::memcpy(dst, tempData.data(), sizeof(float) * 3);
				dst += sizeof(float) * 3;

				if (hasNormals)
				{
					tempData[0] = static_cast<float>(_vertexNormals[i].X);
					tempData[1] = static_cast<float>(_vertexNormals[i].Y);
					tempData[2] = static_cast<float>(_vertexNormals[i].Z);

					std::memcpy(dst, tempData.data(), sizeof(float) * 3);
					dst += sizeof(float) * 3;
				}

				if (hasUV0s)
				{
					tempData[0] = static_cast<float>(_vertexUV0s[i].X);
					tempData[1] = static_cast<float>(_vertexUV0s[i].Y);

					std::memcpy(dst, tempData.data(), sizeof(float) * 2);
				}
			}

			// Upload vertex data
			stagingBuffer->LoadData(0, vertexData);
			stagingBuffer->CopyTo(0, _vertexBuffer.Get(), 0, vertexData.Count());

			// Resize for index data
			stagingBuffer->Resize(indexBufferSize, false);

			// Upload index data
			stagingBuffer->LoadData(0, _vertexIndices);
			stagingBuffer->CopyTo(0, _indexBuffer.Get(), 0, sizeof(uint32_t) * _vertexIndices.Count());

			platform->PurgeResource(stagingBuffer);

			if (deleteLocalData)
			{
				_vertexPositions.Clear();
				_vertexNormals.Clear();
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
