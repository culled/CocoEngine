#include "Renderpch.h"
#include "ShaderUniformDataSerializer.h"

#include <Coco/Core/Engine.h>
#include <yaml-cpp/yaml.h>
#include <Coco/Third Party/yaml-cpp/Converters.h>

namespace Coco::Rendering
{
	void ShaderUniformDataSerializer::Serialize(
		YAML::Emitter& emitter, 
		const ShaderUniformData& data, 
		const std::unordered_map<ShaderUniformData::UniformKey, SharedRef<Texture>>& textures)
	{
		if (data.Uniforms.size() > 0)
		{
			emitter << YAML::Key << "data" << YAML::Value << YAML::BeginMap;

			for (const auto& u : data.Uniforms)
			{
				emitter << YAML::Key << u.first << YAML::Value;
				SerializeUniformUnion(emitter, u.second);
			}

			emitter << YAML::EndMap;
		}

		if (textures.size() > 0)
		{
			emitter << YAML::Key << "textures" << YAML::Value << YAML::BeginMap;
			for (const auto& it : textures)
			{
				if (it.second)
				{
					emitter << YAML::Key << it.first << YAML::Value << it.second->GetContentPath();
				}
			}
			emitter << YAML::EndMap;
		}
	}

	void ShaderUniformDataSerializer::Deserialize(
		const YAML::Node& baseNode, 
		ShaderUniformData& outData, 
		std::unordered_map<ShaderUniformData::UniformKey, SharedRef<Texture>>& outTextures)
	{
		if (baseNode["data"])
		{
			YAML::Node dataNode = baseNode["data"];
			for (YAML::const_iterator it = dataNode.begin(); it != dataNode.end(); ++it)
			{
				ShaderUniformData::UniformKey key = it->first.as<uint64>();
				DeserializeUniformUnion(it->second, outData.Uniforms[key]);
			}
		}

		if (baseNode["textures"])
		{
			ResourceLibrary& resources = Engine::Get()->GetResourceLibrary();

			YAML::Node texturesNode = baseNode["textures"];
			for (YAML::const_iterator it = texturesNode.begin(); it != texturesNode.end(); ++it)
			{
				string texturePath = it->second.as<string>();
				if (texturePath.empty())
					continue;

				ShaderUniformData::UniformKey key = it->first.as<uint64>();
				SharedRef<Texture> tex = resources.GetOrLoad<Texture>(it->second.as<string>());
				if (tex)
				{
					outTextures[key] = tex;
					outData.Textures[key] = ShaderUniformData::ToTextureSampler(tex->GetImage(), tex->GetImageSampler());
				}
			}
		}
	}

	void ShaderUniformDataSerializer::SerializeUniformUnion(YAML::Emitter& emitter, const ShaderUniformUnion& uniformUnion)
	{
		emitter << YAML::Flow << YAML::BeginSeq;

		for (int i = 0; i < ShaderUniformUnion::MaxDataSize; i++)
			emitter << static_cast<int>(uniformUnion.AsData[i]);

		emitter << YAML::EndSeq;
	}

	void ShaderUniformDataSerializer::DeserializeUniformUnion(const YAML::Node& baseNode, ShaderUniformUnion& outUniformUnion)
	{
		for (int i = 0; i < ShaderUniformUnion::MaxDataSize; i++)
			outUniformUnion.AsData[i] = static_cast<uint8>(baseNode[i].as<int>());
	}
}