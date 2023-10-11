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

	const std::type_index MeshSerializer::GetResourceTypeForExtension(const string& extension) const
	{
		return typeid(Mesh);
	}

	string MeshSerializer::Serialize(SharedRef<Resource> resource)
	{
		SharedRef<Mesh> mesh = std::dynamic_pointer_cast<Mesh>(resource);

		Assert(mesh)

		YAML::Emitter out;

		out << YAML::BeginMap;

		out << YAML::Key << "name" << YAML::Value << mesh->GetName();

		out << YAML::Key << "vertexFormat" << YAML::Value << YAML::Flow << YAML::BeginSeq;

		const VertexDataFormat format = mesh->GetVertexFormat();
		out << format.HasNormals << format.HasColor << format.HasTangents << format.HasUV0;

		out << YAML::EndSeq;

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

	SharedRef<Resource> MeshSerializer::Deserialize(const std::type_index& type, const ResourceID& resourceID, const string& data)
	{
		YAML::Node baseNode = YAML::Load(data);

		string name = baseNode["name"].as<string>();

		const YAML::Node vertexFormatNode = baseNode["vertexFormat"];
		VertexDataFormat format;

		format.HasNormals = vertexFormatNode[0].as<bool>();
		format.HasColor = vertexFormatNode[1].as<bool>();
		format.HasTangents = vertexFormatNode[2].as<bool>();
		format.HasUV0 = vertexFormatNode[3].as<bool>();

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

		SharedRef<Mesh> mesh = CreateSharedRef<Mesh>(resourceID, name);
		mesh->SetVertices(format, vertices);

		const YAML::Node indicesNode = baseNode["indices"];
		for (YAML::const_iterator it = indicesNode.begin(); it != indicesNode.end(); it++)
		{
			std::vector<uint32> i = it->second.as<std::vector<uint32>>();
			mesh->SetIndices(i, it->first.as<uint32>());
		}

		mesh->Apply();

		return mesh;
	}
}