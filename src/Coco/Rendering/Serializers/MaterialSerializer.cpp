#include "Renderpch.h"
#include "MaterialSerializer.h"

#include "../Material.h"
//#include "../MaterialInstance.h"
#include "Types/ShaderUniformDataSerializer.h"

#include <yaml-cpp/yaml.h>
#include <Coco/Third Party/yaml-cpp/Converters.h>

#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	bool MaterialSerializer::SupportsFileExtension(const string& extension) const
	{
		return extension == ".cmaterial" || extension == ".cmaterialinst";
	}

	bool MaterialSerializer::SupportsResourceType(const std::type_index& type) const
	{
		//return type == typeid(Material) || type == typeid(MaterialInstance);
		return type == typeid(Material);
	}

	const std::type_index MaterialSerializer::GetResourceTypeForExtension(const string& extension) const
	{
		//return extension == ".cmaterial" ? typeid(Material) : typeid(MaterialInstance);
		return typeid(Material);
	}

	string MaterialSerializer::Serialize(SharedRef<Resource> resource)
	{
		if (SharedRef<Material> material = std::dynamic_pointer_cast<Material>(resource))
			return SerializeMaterial(*material);
		//else if (SharedRef<MaterialInstance> materialInst = std::dynamic_pointer_cast<MaterialInstance>(resource))
		//	return SerializeMaterialInstance(*materialInst);

		throw std::exception("Unsupported resource type");
	}

	SharedRef<Resource> MaterialSerializer::Deserialize(const std::type_index& resourceType, const ResourceID& resourceID, const string& data)
	{
		if (resourceType == typeid(Material))
			return DeserializeMaterial(resourceID, data);
		//else if (resourceType == typeid(MaterialInstance))
		//	return DeserializeMaterialInstance(resourceID, data);

		throw std::exception("Unsupported resource type");
	}

	string MaterialSerializer::SerializeMaterial(const Material& material)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;

		out << YAML::Key << "name" << YAML::Value << material.GetName();

		out << YAML::Key << "parameters" << YAML::Value << YAML::BeginMap;
		material.ForEachParameter(
			[&out](const MaterialParameter& param)
			{
				out << YAML::Key << param.Name << YAML::Value << YAML::BeginMap;

				out << YAML::Key << "type" << YAML::Value << static_cast<int>(param.Type);

				out << YAML::Key << "value" << YAML::Value;

				switch (param.Type)
				{
				case ShaderUniformType::Float:
					out << param.As<float>();
					break;
				case ShaderUniformType::Float2:
					out << param.As<Vector2>();
					break;
				case ShaderUniformType::Float3:
					out << param.As<Vector3>();
					break;
				case ShaderUniformType::Float4:
					out << param.As<Vector4>();
					break;
				case ShaderUniformType::Color:
					out << param.As<Color>();
					break;
				case ShaderUniformType::Mat4x4:
					out << param.As<Matrix4x4>();
					break;
				case ShaderUniformType::Int:
					out << param.As<int>();
					break; 
				case ShaderUniformType::Int2:
					out << param.As<Vector2Int>();
					break;
				case ShaderUniformType::Int3:
					out << param.As<Vector3Int>();
					break;
				case ShaderUniformType::Int4:
					out << param.As<Vector4Int>();
					break;
				case ShaderUniformType::Bool:
					out << param.As<bool>();
					break;
				case ShaderUniformType::Texture:
				{
					SharedRef<Texture> tex = param.As<SharedRef<Texture>>();

					if (tex)
						out << tex->GetContentPath();
					else
						out << "";
				}
				}

				out << YAML::EndMap;
			}
		);

		out << YAML::EndMap;

		out << YAML::EndMap << YAML::Comment("Fix");

		return string(out.c_str());
	}

	SharedRef<Resource> MaterialSerializer::DeserializeMaterial(const ResourceID& resourceID, const string& data)
	{
		YAML::Node baseNode = YAML::Load(data);
		string name = baseNode["name"].as<string>();

		SharedRef<Material> material = CreateSharedRef<Material>(resourceID, name);

		YAML::Node parametersNode = baseNode["parameters"];
		for (YAML::const_iterator it = parametersNode.begin(); it != parametersNode.end(); it++)
		{
			string paramName = it->first["name"].as<string>();

			ShaderUniformType type = static_cast<ShaderUniformType>(it->second["type"].as<int>());
			std::any value;

			YAML::Node valueNode = it->second["value"];
			switch (type)
			{
			case ShaderUniformType::Float:
				value = valueNode.as<float>();
				break;
			case ShaderUniformType::Float2:
				value = valueNode.as<Vector2>();
				break;
			case ShaderUniformType::Float3:
				value = valueNode.as<Vector3>();
				break;
			case ShaderUniformType::Float4:
				value = valueNode.as<Vector4>();
				break;
			case ShaderUniformType::Color:
				value = valueNode.as<Color>();
				break;
			case ShaderUniformType::Mat4x4:
				value = valueNode.as<Matrix4x4>();
				break;
			case ShaderUniformType::Int:
				value = valueNode.as<int>();
				break;
			case ShaderUniformType::Int2:
				value = valueNode.as<Vector2Int>();
				break;
			case ShaderUniformType::Int3:
				value = valueNode.as<Vector3Int>();
				break;
			case ShaderUniformType::Int4:
				value = valueNode.as<Vector4Int>();
				break;
			case ShaderUniformType::Bool:
				value = valueNode.as<bool>();
				break;
			case ShaderUniformType::Texture:
			{
				string texturePath = valueNode.as<string>();

				if (texturePath.empty())
				{
					value = SharedRef<Texture>();
				}
				else
				{
					value = Engine::Get()->GetResourceLibrary().GetOrLoad<Texture>(texturePath);
				}

				break;
			}
			default:
				break;
			}

			material->_parameters[paramName] = MaterialParameter(paramName, type, value);
		}

		return material;
	}

	/*string MaterialSerializer::SerializeMaterialInstance(const MaterialInstance& material)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;

		out << YAML::Key << "name" << YAML::Value << material.GetName();

		SharedRef<Material> baseMaterial = material._baseMaterial;

		out << YAML::Key << "base material path" << YAML::Value << (baseMaterial ? baseMaterial->GetContentPath() : "");

		out << YAML::Key << "uniforms" << YAML::Value << YAML::BeginMap;

		ShaderUniformDataSerializer::Serialize(out, material._uniformData, material._textures);

		out << YAML::EndMap;

		out << YAML::EndMap << YAML::Comment("Fix");

		return string(out.c_str());
	}

	SharedRef<Resource> MaterialSerializer::DeserializeMaterialInstance(const ResourceID& resourceID, const string& data)
	{
		YAML::Node baseNode = YAML::Load(data);
		string name = baseNode["name"].as<string>();
		string baseMaterialPath = baseNode["base material path"].as<string>();

		SharedRef<Material> baseMaterial = nullptr;
		if(!baseMaterialPath.empty())
			baseMaterial = Engine::Get()->GetResourceLibrary().GetOrLoad<Material>(baseMaterialPath);

		SharedRef<MaterialInstance> material = CreateSharedRef<MaterialInstance>(resourceID, name, baseMaterial);

		YAML::Node uniformsNode = baseNode["uniforms"];
		ShaderUniformDataSerializer::Deserialize(uniformsNode, material->_uniformData, material->_textures);

		return material;
	}*/
}