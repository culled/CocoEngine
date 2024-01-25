#include "Renderpch.h"
#include "MeshSerializer.h"
#include "../Mesh.h"

#include <Coco/Core/Types/YAMLConverters.h>
#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	bool MeshSerializer::SerializeYAML(YAML::Emitter& emitter, const SharedRef<Resource>& resource)
	{
		SharedRef<Mesh> mesh = std::dynamic_pointer_cast<Mesh>(resource);

		if (!mesh)
		{
			CocoError("Resource was not a mesh resource")
			return false;
		}

		emitter << YAML::Key << "usageFlags" << YAML::Value << static_cast<int>(mesh->_usageFlags);
		emitter << YAML::Key << "vertexFormat" << YAML::Value << static_cast<int>(mesh->_vertexFormat.AdditionalAttributes);

		// TODO: this should probably be binary data
		emitter << YAML::Key << "vertices" << YAML::Value << YAML::BeginSeq;

		for (const auto& vertex : mesh->GetVertices())
		{
			emitter << YAML::Flow << YAML::BeginSeq;

			emitter << vertex.Position << vertex.Normal << vertex.Color << vertex.Tangent << vertex.UV0 << YAML::EndSeq;
		}

		emitter << YAML::EndSeq;

		emitter << YAML::Key << "indices" << YAML::Value << YAML::Flow << mesh->_indices;

		emitter << YAML::Key << "submeshes" << YAML::Value << YAML::BeginSeq;

		for (const auto& it : mesh->_submeshes)
		{
			emitter << YAML::BeginMap;

			emitter << YAML::Key << "id" << YAML::Value << it.first;
			emitter << YAML::Key << "indexOffset" << YAML::Value << it.second.IndexOffset;
			emitter << YAML::Key << "indexCount" << YAML::Value << it.second.IndexCount;

			emitter << YAML::EndMap;
		}

		emitter << YAML::EndSeq;

		return true;
	}

	SharedRef<Resource> MeshSerializer::CreateResource(const ResourceID& id)
	{
		return CreateSharedRef<Mesh>(id, MeshUsageFlags::None);
	}

	bool MeshSerializer::DeserializeYAML(const YAML::Node& baseNode, SharedRef<Resource> resource)
	{
		SharedRef<Mesh> mesh = std::dynamic_pointer_cast<Mesh>(resource);

		if (!mesh)
		{
			CocoError("Resource was not a mesh resource")
			return false;
		}

		mesh->_usageFlags = static_cast<MeshUsageFlags>(baseNode["usageFlags"].as<int>());
		VertexDataFormat vertexFormat(static_cast<VertexAttrFlags>(baseNode["vertexFormat"].as<int>()));

		std::vector<VertexData> vertices;
		const YAML::Node verticesNode = baseNode["vertices"];
		for (YAML::const_iterator it = verticesNode.begin(); it != verticesNode.end(); it++)
		{
			const YAML::Node& vertexNode = *it;
			VertexData& v = vertices.emplace_back(vertexNode[0].as<Vector3>());
			v.Normal = vertexNode[1].as<Vector3>();
			v.Color = vertexNode[2].as<Vector4>();
			v.Tangent = vertexNode[3].as<Vector4>();
			v.UV0 = vertexNode[4].as<Vector2>();
		}

		mesh->SetVertices(vertexFormat, vertices);

		std::vector<uint32> indices = baseNode["indices"].as<std::vector<uint32>>();

		std::unordered_map<uint32, Submesh> submeshes;
		const YAML::Node submeshesNode = baseNode["submeshes"];
		for (YAML::const_iterator it = submeshesNode.begin(); it != submeshesNode.end(); it++)
		{
			const YAML::Node& submeshNode = *it;

			submeshes.try_emplace(submeshNode["id"].as<uint32>(), submeshNode["indexOffset"].as<uint64>(), submeshNode["indexCount"].as<uint64>());
		}

		mesh->SetIndices(indices, submeshes);
		mesh->IncrementVersion();

		return true;
	}
}