#include "Mesh.h"

#include "RenderingService.h"
#include "Graphics/Resources/BufferTypes.h"

namespace Coco::Rendering
{
	VertexData::VertexData(const Vector3& position) :
		Position(position), Normal(), UV0(), Color(), Tangent()
	{}

	VertexData::VertexData(const Vector3 & position, const Vector3 & normal) :
		Position(position), Normal(normal), UV0(), Color(), Tangent()
	{}

	VertexData::VertexData(const Vector3 & position, const Vector3 & normal, const Vector2 & uv) :
		Position(position), Normal(normal), UV0(uv), Color(), Tangent()
	{}

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

	void Mesh::CalculateNormals(List<VertexData>& vertices, const List<uint32_t> indices)
	{
		UnorderedMap<uint64_t, Vector3> normals;

		for (uint64_t i = 0; i < indices.Count(); i += 3)
		{
			const uint32_t i0 = indices[i];
			const uint32_t i1 = indices[i + 1];
			const uint32_t i2 = indices[i + 2];

			Vector3 edge1 = vertices[i1].Position - vertices[i0].Position;
			Vector3 edge2 = vertices[i2].Position - vertices[i0].Position;

			Vector3 normal = edge1.Cross(edge2).Normalized();

			normals[i0] += normal;
			normals[i1] += normal;
			normals[i2] += normal;
		}

		for (uint64_t i = 0; i < vertices.Count(); i++)
		{
			vertices[i].Normal = normals[i].Normalized();
		}
	}

	bool Mesh::CalculateTangents(List<VertexData>& vertices, const List<uint32_t> indices)
	{
		UnorderedMap<uint64_t, Vector4> tangents;

		// https://stackoverflow.com/questions/5255806/how-to-calculate-tangent-and-binormal
		for (uint64_t i = 0; i < indices.Count(); i += 3)
		{
			const uint32_t i0 = indices[i];
			const uint32_t i1 = indices[i + 1];
			const uint32_t i2 = indices[i + 2];

			if (!vertices[i0].UV0.has_value() || !vertices[i1].UV0.has_value() || !vertices[i2].UV0.has_value())
			{
				LogError(RenderingService::Get()->GetLogger(), "Vertices did not have UV0 information to generate tangent data");
				return false;
			}

			if (!vertices[i0].Normal.has_value() || !vertices[i1].Normal.has_value() || !vertices[i2].Normal.has_value())
			{
				LogError(RenderingService::Get()->GetLogger(), "Vertices did not have normal information to generate tangent data");
				return false;
			}

			Vector3 v1 = vertices[i1].Position - vertices[i0].Position;
			Vector3 v2 = vertices[i2].Position - vertices[i0].Position;

			Vector2 t1 = vertices[i1].UV0.value() - vertices[i0].UV0.value();
			Vector2 t2 = vertices[i2].UV0.value() - vertices[i0].UV0.value();

			Vector3 normal = (vertices[i0].Normal.value() + vertices[i1].Normal.value() + vertices[i2].Normal.value()).Normalized();

			v1 -= normal * v1.Dot(normal);
			v2 -= normal * v2.Dot(normal);

			double handedness = ((t1.X * t2.Y - t1.Y * t2.X) < 0.0) ? -1.0 : 1.0;

			Vector3 s = ((v1 * t2.Y - v2 * t1.Y) * handedness).Normalized();

			double angle = Math::Acos(v1.Dot(v2) / (v1.GetLength() * v2.GetLength()));

			Vector4 tangent4(s, handedness);

			tangents[i0] += tangent4;
			tangents[i1] += tangent4;
			tangents[i2] += tangent4;
		}

		for (uint64_t i = 0; i < vertices.Count(); i++)
		{
			const Vector4& t = tangents[i];
			Vector3 xyz(t.X, t.Y, t.Z);
			xyz.Normalize();

			vertices[i].Tangent = Vector4(xyz, t.W > 0.0 ? 1.0 : -1.0);
		}

		return true;
	}

	void Mesh::SetVertexData(const List<VertexData>& vertices)
	{
		_vertexData = vertices;
		MarkDirty();
	}

	void Mesh::SetPositions(const List<Vector3>& positions)
	{
		_vertexData.Resize(positions.Count());

		for(uint64_t i = 0; i < _vertexData.Count(); i++)
			_vertexData[i].Position = positions[i];

		MarkDirty();
	}

	void Mesh::SetNormals(const List<Vector3>& normals)
	{
		if (normals.Count() != _vertexData.Count())
		{
			LogError(EnsureRenderingService()->GetLogger(), FormattedString(
				"Normal count ({}) does not equal vertex count ({}). Did you set vertex positions first?",
				normals.Count(), _vertexData.Count()
			));

			return;
		}

		for (uint64_t i = 0; i < _vertexData.Count(); i++)
			_vertexData[i].Normal = normals[i];

		MarkDirty();
	}

	void Mesh::SetUVs(const List<Vector2>& uvs)
	{
		if (uvs.Count() != _vertexData.Count())
		{
			LogError(EnsureRenderingService()->GetLogger(), FormattedString(
				"UV count ({}) does not equal vertex count ({}). Did you set vertex positions first?",
				uvs.Count(), _vertexData.Count()
			));

			return;
		}

		for (uint64_t i = 0; i < _vertexData.Count(); i++)
			_vertexData[i].UV0 = uvs[i];

		MarkDirty();
	}

	void Mesh::SetColors(const List<Vector4>& colors)
	{
		if (colors.Count() != _vertexData.Count())
		{
			LogError(EnsureRenderingService()->GetLogger(), FormattedString(
				"Color count ({}) does not equal vertex count ({}). Did you set vertex positions first?",
				colors.Count(), _vertexData.Count()
			));

			return;
		}

		for (uint64_t i = 0; i < _vertexData.Count(); i++)
			_vertexData[i].Color = colors[i];

		MarkDirty();
	}

	void Mesh::SetTangents(const List<Vector4>& tangents)
	{
		if (tangents.Count() != _vertexData.Count())
		{
			LogError(EnsureRenderingService()->GetLogger(), FormattedString(
				"Tangent count ({}) does not equal vertex count ({}). Did you set vertex positions first?",
				tangents.Count(), _vertexData.Count()
			));

			return;
		}

		for (uint64_t i = 0; i < _vertexData.Count(); i++)
			_vertexData[i].Tangent = tangents[i];

		MarkDirty();
	}

	void Mesh::SetIndices(const List<uint32_t>& indices, uint submeshIndex)
	{
		if (submeshIndex >= _submeshes.Count())
			_submeshes.Resize(submeshIndex + 1);

		Submesh& submesh = _submeshes[submeshIndex];
		submesh.Indices = indices;

		MarkDirty();
	}

	void Mesh::EnsureChannels(bool normal, bool uv, bool color, bool tangent)
	{
		for (VertexData& vertex : _vertexData)
		{
			if (normal && !vertex.Normal.has_value())
				vertex.Normal = Vector3::Zero;

			if (uv && !vertex.UV0.has_value())
				vertex.UV0 = Vector2::Zero;

			if (color && !vertex.Color.has_value())
				vertex.Color = Vector4::Zero;

			if (tangent && !vertex.Tangent.has_value())
				vertex.Tangent = Vector4::Zero;
		}

		MarkDirty();
	}

	bool Mesh::UploadData(bool deleteLocalData)
	{
		if (!_isDirty)
			return true;

		GraphicsPlatform* platform = EnsureRenderingService()->GetPlatform();
		Ref<Buffer> stagingBuffer = Ref<Buffer>::Empty;

		try
		{
			if (_vertexData.Count() == 0)
				throw InvalidOperationException("No vertex data has been set");

			Ref<Buffer> stagingBuffer = platform->CreateBuffer(FormattedString("{} staging buffer", _name),
				_vertexData.Count(),
				BufferUsageFlags::TransferSource | BufferUsageFlags::TransferDestination | BufferUsageFlags::HostVisible,
				true);

			UploadVertexBufferData(stagingBuffer, deleteLocalData);
			UploadIndexBufferData(stagingBuffer, deleteLocalData);	
			platform->PurgeResource(stagingBuffer);
			IncrementVersion();

			_isDirty = false;

			return true;
		}
		catch (const Exception& ex)
		{
			LogError(platform->GetLogger(), FormattedString(
				"Failed to upload mesh data: {}",
				ex.what()
			));

			if (stagingBuffer.IsValid())
				platform->PurgeResource(stagingBuffer);

			_vertexCount = 0;
			_indexCount = 0;

			return false;
		}
	}

	void Mesh::CalculateNormals()
	{
		for(const auto& submesh : _submeshes)
			CalculateNormals(_vertexData, submesh.Indices);

		MarkDirty();
	}

	bool Mesh::CalculateTangents()
	{
		for (const auto& submesh : _submeshes)
			if (!CalculateTangents(_vertexData, submesh.Indices))
				return false;

		MarkDirty();

		return true;
	}

	void Mesh::UploadVertexBufferData(Ref<Buffer> stagingBuffer, bool deleteVertexData)
	{
		bool hasNormals = _vertexData[0].Normal.has_value();
		bool hasUV0s = _vertexData[0].UV0.has_value();
		bool hasColors = _vertexData[0].Color.has_value();
		bool hasTangents = _vertexData[0].Tangent.has_value();

		uint64_t vertexStride = sizeof(float) * 3;

		if (hasNormals)
			vertexStride += sizeof(float) * 3;

		if (hasUV0s)
			vertexStride += sizeof(float) * 2;

		if (hasColors)
			vertexStride += sizeof(float) * 4;

		if (hasTangents)
			vertexStride += sizeof(float) * 4;

		_vertexCount = _vertexData.Count();
		const uint64_t vertexBufferSize = _vertexCount * vertexStride;

		Array<float, 4> tempData = { 0.0f };

		// Build the vertex data
		List<char> vertexData(vertexBufferSize);
		for (uint64_t i = 0; i < _vertexCount; i++)
		{
			const VertexData& vertex = _vertexData[i];

			uint64_t offset = i * vertexStride;
			char* dst = vertexData.Data() + offset;

			tempData[0] = static_cast<float>(vertex.Position.X);
			tempData[1] = static_cast<float>(vertex.Position.Y);
			tempData[2] = static_cast<float>(vertex.Position.Z);

			std::memcpy(dst, tempData.data(), sizeof(float) * 3);
			dst += sizeof(float) * 3;

			if (hasNormals)
			{
				tempData[0] = static_cast<float>(vertex.Normal.value().X);
				tempData[1] = static_cast<float>(vertex.Normal.value().Y);
				tempData[2] = static_cast<float>(vertex.Normal.value().Z);

				std::memcpy(dst, tempData.data(), sizeof(float) * 3);
				dst += sizeof(float) * 3;
			}

			if (hasUV0s)
			{
				tempData[0] = static_cast<float>(vertex.UV0.value().X);
				tempData[1] = static_cast<float>(vertex.UV0.value().Y);

				std::memcpy(dst, tempData.data(), sizeof(float) * 2);

				dst += sizeof(float) * 2;
			}

			if (hasColors)
			{
				tempData[0] = static_cast<float>(vertex.Color.value().X);
				tempData[1] = static_cast<float>(vertex.Color.value().Y);
				tempData[2] = static_cast<float>(vertex.Color.value().Z);
				tempData[3] = static_cast<float>(vertex.Color.value().W);

				std::memcpy(dst, tempData.data(), sizeof(float) * 4);
				dst += sizeof(float) * 4;
			}

			if (hasTangents)
			{
				tempData[0] = static_cast<float>(vertex.Tangent.value().X);
				tempData[1] = static_cast<float>(vertex.Tangent.value().Y);
				tempData[2] = static_cast<float>(vertex.Tangent.value().Z);
				tempData[3] = static_cast<float>(vertex.Tangent.value().W);

				std::memcpy(dst, tempData.data(), sizeof(float) * 4);
				dst += sizeof(float) * 4;
			}
		}

		if (!_vertexBuffer.IsValid())
			_vertexBuffer = EnsureRenderingService()->GetPlatform()->CreateBuffer(FormattedString("{} vertex buffer", _name),
				vertexBufferSize,
				BufferUsageFlags::TransferDestination | BufferUsageFlags::TransferSource | BufferUsageFlags::Vertex,
				true);

		stagingBuffer->Resize(vertexBufferSize, false);

		// Upload vertex data
		stagingBuffer->LoadData(0, vertexData);
		stagingBuffer->CopyTo(0, _vertexBuffer.Get(), 0, vertexBufferSize);

		if (deleteVertexData)
			_vertexData.Clear();
	}

	void Mesh::UploadIndexBufferData(Ref<Buffer> stagingBuffer, bool deleteSubmeshIndices)
	{
		List<uint32_t> indexData;

		_indexCount = 0;
		uint i = 0;

		for (auto& submesh : _submeshes)
		{
			_indexCount += submesh.Indices.Count();

			submesh.IndexBufferOffset = static_cast<uint32_t>(indexData.Count());
			submesh.IndexCount = static_cast<uint32_t>(submesh.Indices.Count());

			indexData.Resize(_indexCount);

			std::memcpy(indexData.Data() + submesh.IndexBufferOffset, submesh.Indices.Data(), submesh.IndexCount * sizeof(uint32_t));

			i++;
		}

		const uint64_t indexBufferSize = sizeof(uint32_t) * _indexCount;

		if (!_indexBuffer.IsValid())
			_indexBuffer = EnsureRenderingService()->GetPlatform()->CreateBuffer(FormattedString("{} index buffer", _name),
				indexBufferSize,
				BufferUsageFlags::TransferDestination | BufferUsageFlags::TransferSource | BufferUsageFlags::Index,
				true);

		// Resize for index data
		stagingBuffer->Resize(indexBufferSize, false);

		// Upload index data
		stagingBuffer->LoadData(0, indexData);
		stagingBuffer->CopyTo(0, _indexBuffer.Get(), 0, indexBufferSize);

		if (deleteSubmeshIndices)
		{
			for (auto& submesh : _submeshes)
				submesh.Indices.Clear();
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
