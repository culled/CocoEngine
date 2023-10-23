#include "Renderpch.h"
#include "ShaderUniformDataSerializer.h"

#include <Coco/Core/Engine.h>
#include <yaml-cpp/yaml.h>

namespace Coco::Rendering
{
	void EmitFloatArray(YAML::Emitter& out, const ShaderUniformData::UniformKey& key, std::span<const float> values)
	{
		out << YAML::Key << key << YAML::Value << YAML::Flow << YAML::BeginSeq;

		for (const float f : values)
			out << f;

		out << YAML::EndSeq;
	}

	void EmitIntArray(YAML::Emitter& out, const ShaderUniformData::UniformKey& key, std::span<const int32> values)
	{
		out << YAML::Key << key << YAML::Value << YAML::Flow << YAML::BeginSeq;

		for (const int32 f : values)
			out << f;

		out << YAML::EndSeq;
	}

	void ReadFloatArray(const YAML::Node& baseNode, std::span<float> values)
	{
		uint64 i = 0;
		for (YAML::const_iterator it = baseNode.begin(); it != baseNode.end() && i < values.size(); ++it)
			values[i++] = it->as<float>();
	}

	void ReadIntArray(const YAML::Node& baseNode, std::span<int32> values)
	{
		uint64 i = 0;
		for (YAML::const_iterator it = baseNode.begin(); it != baseNode.end() && i < values.size(); ++it)
			values[i++] = it->as<int32>();
	}

	void ShaderUniformDataSerializer::Serialize(
		YAML::Emitter& emitter, 
		const ShaderUniformData& data, 
		const std::unordered_map<ShaderUniformData::UniformKey, SharedRef<Texture>>& textures)
	{
		if (data.Floats.size() > 0)
		{
			emitter << YAML::Key << "floats" << YAML::Value << YAML::BeginMap;
			for (const auto& it : data.Floats)
			{
				emitter << YAML::Key << it.first << YAML::Value << it.second;
			}
			emitter << YAML::EndMap;
		}

		if (data.Float2s.size() > 0)
		{
			emitter << YAML::Key << "float2s" << YAML::Value << YAML::BeginMap;
			for (const auto& it : data.Float2s)
			{
				EmitFloatArray(emitter, it.first, it.second);
			}
			emitter << YAML::EndMap;
		}

		if (data.Float3s.size() > 0)
		{
			emitter << YAML::Key << "float3s" << YAML::Value << YAML::BeginMap;
			for (const auto& it : data.Float3s)
			{
				EmitFloatArray(emitter, it.first, it.second);
			}
			emitter << YAML::EndMap;
		}

		if (data.Float4s.size() > 0)
		{
			emitter << YAML::Key << "float4s" << YAML::Value << YAML::BeginMap;
			for (const auto& it : data.Float4s)
			{
				EmitFloatArray(emitter, it.first, it.second);
			}
			emitter << YAML::EndMap;
		}

		if (data.Mat4x4s.size() > 0)
		{
			emitter << YAML::Key << "mat4x4s" << YAML::Value << YAML::BeginMap;
			for (const auto& it : data.Mat4x4s)
			{
				EmitFloatArray(emitter, it.first, it.second);
			}
			emitter << YAML::EndMap;
		}

		if (data.Ints.size() > 0)
		{
			emitter << YAML::Key << "ints" << YAML::Value << YAML::BeginMap;
			for (const auto& it : data.Ints)
			{
				emitter << YAML::Key << it.first << YAML::Value << it.second;
			}
			emitter << YAML::EndMap;
		}

		if (data.Int2s.size() > 0)
		{
			emitter << YAML::Key << "int2s" << YAML::Value << YAML::BeginMap;
			for (const auto& it : data.Int2s)
			{
				EmitIntArray(emitter, it.first, it.second);
			}
			emitter << YAML::EndMap;
		}

		if (data.Int3s.size() > 0)
		{
			emitter << YAML::Key << "int3s" << YAML::Value << YAML::BeginMap;
			for (const auto& it : data.Int3s)
			{
				EmitIntArray(emitter, it.first, it.second);
			}
			emitter << YAML::EndMap;
		}

		if (data.Int4s.size() > 0)
		{
			emitter << YAML::Key << "int4s" << YAML::Value << YAML::BeginMap;
			for (const auto& it : data.Int4s)
			{
				EmitIntArray(emitter, it.first, it.second);
			}
			emitter << YAML::EndMap;
		}

		if (data.Bools.size() > 0)
		{
			emitter << YAML::Key << "bools" << YAML::Value << YAML::BeginMap;
			for (const auto& it : data.Bools)
			{
				emitter << YAML::Key << it.first << YAML::Value << it.second;
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
		if (baseNode["floats"])
		{
			YAML::Node floatsNode = baseNode["floats"];
			for (YAML::const_iterator it = floatsNode.begin(); it != floatsNode.end(); ++it)
			{
				ShaderUniformData::UniformKey key = it->first.as<uint64>();
				outData.Floats[key] = it->second.as<float>();
			}
		}

		if (baseNode["float2s"])
		{
			YAML::Node float2sNode = baseNode["float2s"];
			for (YAML::const_iterator it = float2sNode.begin(); it != float2sNode.end(); ++it)
			{
				ShaderUniformData::UniformKey key = it->first.as<uint64>();
				ReadFloatArray(it->second, outData.Float2s[key]);
			}
		}

		if (baseNode["float3s"])
		{
			YAML::Node float3sNode = baseNode["float3s"];
			for (YAML::const_iterator it = float3sNode.begin(); it != float3sNode.end(); ++it)
			{
				ShaderUniformData::UniformKey key = it->first.as<uint64>();
				ReadFloatArray(it->second, outData.Float3s[key]);
			}
		}

		if (baseNode["float4s"])
		{
			YAML::Node float4sNode = baseNode["float4s"];
			for (YAML::const_iterator it = float4sNode.begin(); it != float4sNode.end(); ++it)
			{
				ShaderUniformData::UniformKey key = it->first.as<uint64>();
				ReadFloatArray(it->second, outData.Float4s[key]);
			}
		}

		if (baseNode["mat4x4s"])
		{
			YAML::Node mat4x4sNode = baseNode["mat4x4s"];
			for (YAML::const_iterator it = mat4x4sNode.begin(); it != mat4x4sNode.end(); ++it)
			{
				ShaderUniformData::UniformKey key = it->first.as<uint64>();
				ReadFloatArray(it->second, outData.Mat4x4s[key]);
			}
		}

		if (baseNode["ints"])
		{
			YAML::Node intsNode = baseNode["ints"];
			for (YAML::const_iterator it = intsNode.begin(); it != intsNode.end(); ++it)
			{
				ShaderUniformData::UniformKey key = it->first.as<uint64>();
				outData.Ints[key] = it->second.as<int32>();
			}
		}

		if (baseNode["int2s"])
		{
			YAML::Node int2sNode = baseNode["int2s"];
			for (YAML::const_iterator it = int2sNode.begin(); it != int2sNode.end(); ++it)
			{
				ShaderUniformData::UniformKey key = it->first.as<uint64>();
				ReadIntArray(it->second, outData.Int2s[key]);
			}
		}

		if (baseNode["int3s"])
		{
			YAML::Node int3sNode = baseNode["int3s"];
			for (YAML::const_iterator it = int3sNode.begin(); it != int3sNode.end(); ++it)
			{
				ShaderUniformData::UniformKey key = it->first.as<uint64>();
				ReadIntArray(it->second, outData.Int3s[key]);
			}
		}

		if (baseNode["int4s"])
		{
			YAML::Node int4sNode = baseNode["int4s"];
			for (YAML::const_iterator it = int4sNode.begin(); it != int4sNode.end(); ++it)
			{
				ShaderUniformData::UniformKey key = it->first.as<uint64>();
				ReadIntArray(it->second, outData.Int4s[key]);
			}
		}

		if (baseNode["bools"])
		{
			YAML::Node boolsNode = baseNode["bools"];
			for (YAML::const_iterator it = boolsNode.begin(); it != boolsNode.end(); ++it)
			{
				ShaderUniformData::UniformKey key = it->first.as<uint64>();
				outData.Bools[key] = it->second.as<bool>();
			}
		}

		if (baseNode["textures"])
		{
			ResourceLibrary& resources = Engine::Get()->GetResourceLibrary();

			YAML::Node texturesNode = baseNode["textures"];
			for (YAML::const_iterator it = texturesNode.begin(); it != texturesNode.end(); ++it)
			{
				ShaderUniformData::UniformKey key = it->first.as<uint64>();
				SharedRef<Texture> tex = resources.GetOrLoad<Texture>(it->second.as<string>());
				outTextures[key] = tex;
				outData.Textures[key] = ShaderUniformData::ToTextureSampler(tex->GetImage(), tex->GetImageSampler());
			}
		}
	}
}