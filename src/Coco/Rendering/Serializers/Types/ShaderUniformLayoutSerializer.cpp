#include "Renderpch.h"
#include "ShaderUniformLayoutSerializer.h"
#include "ShaderUniformDataSerializer.h"

#include <yaml-cpp/yaml.h>

namespace Coco::Rendering
{
	void ShaderUniformLayoutSerialization::SerializeLayout(YAML::Emitter& emitter, const ShaderUniformLayout& layout)
	{
		emitter << YAML::Key << "uniforms" << YAML::Value << YAML::BeginSeq;

		for (const ShaderUniform& u : layout.Uniforms)
		{
			emitter << YAML::BeginMap;

			emitter << YAML::Key << "name" << YAML::Value << u.Name;
			emitter << YAML::Key << "type" << YAML::Value << static_cast<int>(u.Type);
			emitter << YAML::Key << "binding points" << YAML::Value << static_cast<int>(u.BindingPoints);
			emitter << YAML::Key << "default" << YAML::Value;

			if (IsDataShaderUniformType(u.Type))
			{
				ShaderUniformUnion defaultValue = layout.GetDefaultDataUniformValue(u);
				ShaderUniformDataSerializer::SerializeUniformUnion(emitter, defaultValue);
			}
			else
			{
				if (const DefaultTextureType* defaultTexture = std::any_cast<DefaultTextureType>(&u.DefaultValue))
				{
					emitter << static_cast<int>(*defaultTexture);
				}
				else
				{
					emitter << "error";
				}
			}

			emitter << YAML::EndMap;
		}

		emitter << YAML::EndSeq;
	}

	void ShaderUniformLayoutSerialization::SerializeGlobalLayout(YAML::Emitter& emitter, const GlobalShaderUniformLayout& layout)
	{
		SerializeLayout(emitter, layout);

		emitter << YAML::Key << "buffer uniforms" << YAML::Value << YAML::BeginSeq;

		for (const ShaderBufferUniform& u : layout.BufferUniforms)
		{
			emitter << YAML::BeginMap;

			emitter << YAML::Key << "name" << YAML::Value << u.Name;
			emitter << YAML::Key << "binding points" << YAML::Value << static_cast<int>(u.BindingPoints);
			emitter << YAML::Key << "size" << YAML::Value << static_cast<int>(u.Size);

			emitter << YAML::EndMap;
		}

		emitter << YAML::EndSeq;
	}

	void ShaderUniformLayoutSerialization::DeserializeLayout(const YAML::Node& baseNode, ShaderUniformLayout& outLayout)
	{
		YAML::Node dataUniformsNode = baseNode["uniforms"];

		for (YAML::const_iterator it = dataUniformsNode.begin(); it != dataUniformsNode.end(); ++it)
		{
			std::any defaultValue;
			ShaderUniformType type = static_cast<ShaderUniformType>((*it)["type"].as<int>());

			YAML::Node defaultNode = (*it)["default"];

			if (IsDataShaderUniformType(type))
			{
				ShaderUniformUnion u;
				ShaderUniformDataSerializer::DeserializeUniformUnion(defaultNode, u);

				switch (type)
				{
				case ShaderUniformType::Bool:
					defaultValue = u.As<bool>();
					break;
				case ShaderUniformType::Float:
					defaultValue = u.As<float>();
					break;
				case ShaderUniformType::Float2:
					defaultValue = u.As<Vector2>();
					break;
				case ShaderUniformType::Float3:
					defaultValue = u.As<Vector3>();
					break;
				case ShaderUniformType::Float4:
					defaultValue = u.As<Vector4>();
					break;
				case ShaderUniformType::Color:
					defaultValue = u.As<Color>();
					break;
				case ShaderUniformType::Mat4x4:
					defaultValue = u.As<Matrix4x4>();
					break;
				case ShaderUniformType::Int:
					defaultValue = u.As<int>();
					break;
				case ShaderUniformType::Int2:
					defaultValue = u.As<Vector2Int>();
					break;
				case ShaderUniformType::Int3:
					defaultValue = u.As<Vector3Int>();
					break;
				case ShaderUniformType::Int4:
					defaultValue = u.As<Vector4Int>();
					break;
				default:
					Assert(false)
					break;
				}
			}
			else
			{
				defaultValue = static_cast<DefaultTextureType>(defaultNode.as<int>());
			}

			outLayout.Uniforms.emplace_back(
				(*it)["name"].as<string>(),
				type,
				static_cast<ShaderStageFlags>((*it)["binding points"].as<int>()),
				defaultValue
			);
		}
	}

	void ShaderUniformLayoutSerialization::DeserializeGlobalLayout(const YAML::Node& baseNode, GlobalShaderUniformLayout& outLayout)
	{
		DeserializeLayout(baseNode, outLayout);

		YAML::Node bufferUniformsNode = baseNode["buffer uniforms"];

		for (YAML::const_iterator it = bufferUniformsNode.begin(); it != bufferUniformsNode.end(); ++it)
		{
			outLayout.BufferUniforms.emplace_back(
				(*it)["name"].as<string>(),
				static_cast<ShaderStageFlags>((*it)["binding points"].as<int>()),
				(*it)["size"].as<uint64>()
			);
		}
	}
}