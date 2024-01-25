#include "Renderpch.h"
#include "MaterialSerializer.h"

#include "../Material.h"
#include "../Texture.h"
#include <Coco/Core/Types/YAMLConverters.h>
#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	bool MaterialSerializer::SerializeYAML(YAML::Emitter& emitter, const SharedRef<Resource>& resource)
	{
		SharedRef<Material> material = std::dynamic_pointer_cast<Material>(resource);
		if (!material)
		{
			CocoError("Resource was not a material resource")
			return false;
		}

		emitter << YAML::Key << "parameters" << YAML::Value << YAML::BeginMap;

		for (const auto& value : material->GetUniformValues())
		{
			emitter << YAML::Key << value.Name << YAML::Value << YAML::BeginMap;

			emitter << YAML::Key << "type" << YAML::Value << static_cast<int>(value.Type);

			emitter << YAML::Key << "value" << YAML::Value;

			switch (value.Type)
			{
			case ShaderUniformType::Float:
				emitter << value.As<float>();
				break;
			case ShaderUniformType::Float2:
				emitter << static_cast<std::span<const float>>(value.As<Float2>());
				break;
			case ShaderUniformType::Float3:
				emitter << static_cast<std::span<const float>>(value.As<Float3>());
				break;
			case ShaderUniformType::Float4:
			case ShaderUniformType::Color:
				emitter << static_cast<std::span<const float>>(value.As<Float4>());
				break;
			case ShaderUniformType::Matrix4x4:
				emitter << static_cast<std::span<const float>>(value.As<FloatMatrix4x4>());
				break;
			case ShaderUniformType::Int:
				emitter << value.As<int>();
				break;
			case ShaderUniformType::Int2:
				emitter << static_cast<std::span<const int>>(value.As<Int2>());
				break;
			case ShaderUniformType::Int3:
				emitter << static_cast<std::span<const int>>(value.As<Int3>());
				break;
			case ShaderUniformType::Int4:
				emitter << static_cast<std::span<const int>>(value.As<Int4>());
				break;
			case ShaderUniformType::Bool:
				emitter << value.As<bool>();
				break;
			case ShaderUniformType::Texture:
			{
				SharedRef<Texture> tex = value.As<SharedRef<Texture>>();

				if (tex)
				{
					emitter << *tex;
				}
				else
				{
					emitter << "";
				}

				break;
			}
			default:
				CocoWarn("Parameter \"{}\" is not a known ShaderUniformValue type", value.Name)
				break;
			}

			emitter << YAML::EndMap;
		}

		emitter << YAML::EndMap;

		return true;
	}

	SharedRef<Resource> MaterialSerializer::CreateResource(const ResourceID& id)
	{
		return CreateSharedRef<Material>(id);
	}

	bool MaterialSerializer::DeserializeYAML(const YAML::Node& baseNode, SharedRef<Resource> resource)
	{
		SharedRef<Material> material = std::dynamic_pointer_cast<Material>(resource);
		if (!material)
		{
			CocoError("Resource was not a material resource")
			return false;
		}

		material->_parameters.clear();
		YAML::Node parametersNode = baseNode["parameters"];

		for (YAML::const_iterator it = parametersNode.begin(); it != parametersNode.end(); it++)
		{
			string paramName = it->first.as<string>();
			ShaderUniformType type = static_cast<ShaderUniformType>(it->second["type"].as<int>());
			YAML::Node valueNode = it->second["value"];
			std::any value;

			switch (type)
			{
			case ShaderUniformType::Float:
				value = valueNode.as<float>();
				break;
			case ShaderUniformType::Float2:
				value = valueNode.as<Float2>();
				break;
			case ShaderUniformType::Float3:
				value = valueNode.as<Float3>();
				break;
			case ShaderUniformType::Float4:
			case ShaderUniformType::Color:
				value = valueNode.as<Float4>();
				break;
			case ShaderUniformType::Matrix4x4:
				value = valueNode.as<FloatMatrix4x4>();
				break;
			case ShaderUniformType::Int:
				value = valueNode.as<int>();
				break;
			case ShaderUniformType::Int2:
				value = valueNode.as<Int2>();
				break;
			case ShaderUniformType::Int3:
				value = valueNode.as<Int3>();
				break;
			case ShaderUniformType::Int4:
				value = valueNode.as<Int4>();
				break;
			case ShaderUniformType::Bool:
				value = valueNode.as<bool>();
				break;
			case ShaderUniformType::Texture:
			{
				SharedRef<Texture> tex = std::dynamic_pointer_cast<Texture>(LoadResourceFromYAML(valueNode));

				if (!tex)
				{
					CocoWarn("Could not load texture for parameter \"{}\"", paramName)
				}

				value = tex;
				break;
			}
			default:
				CocoWarn("Parameter \"{}\" is not a known ShaderUniformValue type", paramName)
				break;
			}

			material->_parameters.emplace_back(paramName, type, value);
		}

		material->IncrementVersion();

		return true;
	}
}