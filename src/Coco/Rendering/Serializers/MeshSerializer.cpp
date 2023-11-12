#include "Renderpch.h"
#include "MeshSerializer.h"

#include "../Mesh.h"

#include <yaml-cpp/yaml.h>
#include <Coco/Third Party/yaml-cpp/Converters.h>

namespace Coco::Rendering
{
	bool MeshSerializer::SupportsFileExtension(const string& extension) const
	{
		return extension == ".cmesh";
	}

	bool MeshSerializer::SupportsResourceType(const std::type_index& type) const
	{
		return type == typeid(Mesh);
	}

	string MeshSerializer::Serialize(SharedRef<Resource> resource)
	{
		SharedRef<Mesh> mesh = std::dynamic_pointer_cast<Mesh>(resource);

		Assert(mesh)

		YAML::Emitter out;

		out << YAML::BeginMap;

		out << YAML::Key << "keepLocalData" << YAML::Value << mesh->_keepLocalData;
		out << YAML::Key << "isDynamic" << YAML::Value << mesh->_isDynamic;

		const VertexDataFormat format = mesh->GetVertexFormat();
		out << YAML::Key << "vertexFormat" << YAML::Value << static_cast<int>(format.AdditionalAttributes);

		// TODO: this should probably be binary data
		out << YAML::Key << "vertices" << YAML::Value << YAML::BeginSeq;
		for(const auto& vertex : mesh->GetVertices())
		{
			out << YAML::Flow << YAML::BeginSeq;

			out << vertex.Position << vertex.Normal << vertex.Color << vertex.Tangent << vertex.UV0 << YAML::EndSeq;
		}

		out << YAML::EndSeq;

		out << YAML::Key << "indices" << YAML::Value << YAML::BeginMap;
		for (const auto& it : mesh->GetIndices())
		{
			out << YAML::Key << it.first << YAML::Value << YAML::Flow << it.second;
		}

		out << YAML::EndMap;

		out << YAML::EndMap << YAML::Comment("Fix");

		return out.c_str();
	}

	SharedRef<Resource> MeshSerializer::CreateAndDeserialize(const ResourceID& id, const string& name, const string& data)
	{
		SharedRef<Mesh> mesh = CreateSharedRef<Mesh>(id, name);
		Deserialize(data, mesh);

		return mesh;
	}

	bool MeshSerializer::Deserialize(const string& data, SharedRef<Resource> resource)
	{
		SharedRef<Mesh> mesh = std::dynamic_pointer_cast<Mesh>(resource);

		Assert(mesh)

		YAML::Node baseNode = YAML::Load(data);

		mesh->_keepLocalData = baseNode["keepLocalData"].as<bool>();
		mesh->_isDynamic = baseNode["isDynamic"].as<bool>();

		VertexDataFormat format(static_cast<VertexAttrFlags>(baseNode["vertexFormat"].as<int>()));

		std::vector<VertexData> vertices;
		const YAML::Node verticesNode = baseNode["vertices"];
		for (YAML::const_iterator it = verticesNode.begin(); it != verticesNode.end(); it++)
		{
			const YAML::Node vertexNode = *it;
			VertexData& v = vertices.emplace_back(vertexNode[0].as<Vector3>());
			v.Normal = vertexNode[1].as<Vector3>();
			v.Color = vertexNode[2].as<Vector4>();
			v.Tangent = vertexNode[3].as<Vector4>();
			v.UV0 = vertexNode[4].as<Vector2>();
		}

		mesh->SetVertices(format, vertices);

		const YAML::Node indicesNode = baseNode["indices"];
		for (YAML::const_iterator it = indicesNode.begin(); it != indicesNode.end(); it++)
		{
			std::vector<uint32> i = it->second.as<std::vector<uint32>>();
			mesh->SetIndices(i, it->first.as<uint32>());
		}

		mesh->SetVersion(mesh->GetVersion() + 1);
		mesh->Apply();

		return true;
	}
}