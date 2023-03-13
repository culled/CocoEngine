#include "Mesh.h"

#include "RenderingService.h"
#include <Coco/Core/Engine.h>
#include "Graphics/BufferTypes.h"

namespace Coco::Rendering
{
	const uint64_t Mesh::VertexBufferSize = sizeof(VertexData) * 1024 * 1024;
	const uint64_t Mesh::IndexBufferSize = sizeof(uint32_t) * 1024 * 1024;

	Mesh::Mesh()
	{
		if (!Engine::Get()->GetServiceManager()->TryFindService(&_renderingService))
			throw Exception("Cannot create a mesh without an active rendering service");
		
		_vertexBuffer = _renderingService->CreateBuffer(
			VertexBufferSize, 
			BufferUsageFlags::TransferDestination | BufferUsageFlags::TransferSource | BufferUsageFlags::Vertex,
			true);

		_indexBuffer = _renderingService->CreateBuffer(
			IndexBufferSize, 
			BufferUsageFlags::TransferDestination | BufferUsageFlags::TransferSource | BufferUsageFlags::Index, 
			true);
	}

	Mesh::~Mesh()
	{
		_vertexBuffer.reset();
		_indexBuffer.reset();
	}

	void Mesh::SetPositions(const List<Vector3>& positions)
	{
		_vertexPositions = positions;
		_isDirty = true;
	}

	void Mesh::SetIndices(const List<uint32_t>& indices)
	{
		_vertexIndices = indices;
		_isDirty = true;
	}

	void Mesh::UploadData(bool deleteLocalData)
	{
		if (!_isDirty)
			return;

		GraphicsResourceRef<Buffer> staging = _renderingService->CreateBuffer(
			VertexBufferSize,
			BufferUsageFlags::TransferSource | BufferUsageFlags::TransferDestination | BufferUsageFlags::HostVisible,
			true);

		List<VertexData> vertexData(_vertexPositions.Count());
		for (uint64_t i = 0; i < vertexData.Count(); i++)
		{
			vertexData[i].Position[0] = static_cast<float>(_vertexPositions[i].X);
			vertexData[i].Position[1] = static_cast<float>(_vertexPositions[i].Y);
			vertexData[i].Position[2] = static_cast<float>(_vertexPositions[i].Z);
		}

		staging->LoadData(0, vertexData);
		staging->CopyTo(0, _vertexBuffer.get(), 0, sizeof(VertexData) * vertexData.Count());

		_vertexCount = vertexData.Count();

		staging->Resize(IndexBufferSize);

		staging->LoadData(0, _vertexIndices);
		staging->CopyTo(0, _indexBuffer.get(), 0, sizeof(uint32_t) * _vertexIndices.Count());

		_indexCount = _vertexIndices.Count();

		staging.reset();

		if (deleteLocalData)
		{
			_vertexPositions.Clear();
			_vertexIndices.Clear();
		}

		_isDirty = false;
	}
}
