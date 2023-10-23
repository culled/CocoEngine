#include "Renderpch.h"
#include "ShaderUniformLayoutSerializer.h"

#include <yaml-cpp/yaml.h>

namespace Coco::Rendering
{
	void ShaderUniformLayoutSerialization::SerializeLayout(YAML::Emitter& emitter, const ShaderUniformLayout& layout)
	{
		emitter << YAML::Key << "data uniforms" << YAML::Value << YAML::BeginSeq;

		for (const ShaderDataUniform& u : layout.DataUniforms)
		{
			emitter << YAML::BeginMap;

			SerializeBaseUniform(emitter, u);
			emitter << YAML::Key << "type" << YAML::Value << static_cast<int>(u.Type);

			emitter << YAML::EndMap;
		}

		emitter << YAML::EndSeq;

		emitter << YAML::Key << "texture uniforms" << YAML::Value << YAML::BeginSeq;

		for (const ShaderTextureUniform& u : layout.TextureUniforms)
		{
			emitter << YAML::BeginMap;

			SerializeBaseUniform(emitter, u);
			emitter << YAML::Key << "default" << YAML::Value << static_cast<int>(u.DefaultTexture);

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

			SerializeBaseUniform(emitter, u);
			emitter << YAML::Key << "size" << YAML::Value << static_cast<int>(u.Size);

			emitter << YAML::EndMap;
		}

		emitter << YAML::EndSeq;
	}

	ShaderUniformLayout ShaderUniformLayoutSerialization::DeserializeLayout(const YAML::Node& baseNode)
	{
		std::vector<ShaderDataUniform> dataUniforms;
		std::vector<ShaderTextureUniform> textureUniforms;

		DeserializeBaseUniformData(baseNode, dataUniforms, textureUniforms);

		return ShaderUniformLayout(dataUniforms, textureUniforms);
	}

	GlobalShaderUniformLayout ShaderUniformLayoutSerialization::DeserializeGlobalLayout(const YAML::Node& baseNode)
	{
		std::vector<ShaderDataUniform> dataUniforms;
		std::vector<ShaderTextureUniform> textureUniforms;

		DeserializeBaseUniformData(baseNode, dataUniforms, textureUniforms);

		std::vector<ShaderBufferUniform> bufferUniforms;

		YAML::Node bufferUniformsNode = baseNode["buffer uniforms"];

		for (YAML::const_iterator it = bufferUniformsNode.begin(); it != bufferUniformsNode.end(); ++it)
		{
			bufferUniforms.emplace_back(
				(*it)["name"].as<string>(),
				static_cast<ShaderStageFlags>((*it)["binding points"].as<int>()),
				(*it)["size"].as<uint64>()
			);
		}

		return GlobalShaderUniformLayout(dataUniforms, textureUniforms, bufferUniforms);
	}

	void ShaderUniformLayoutSerialization::SerializeBaseUniform(YAML::Emitter& emitter, const ShaderUniform& uniform)
	{
		emitter << YAML::Key << "name" << YAML::Value << uniform.Name;
		emitter << YAML::Key << "binding points" << YAML::Value << static_cast<int>(uniform.BindingPoints);
	}

	void ShaderUniformLayoutSerialization::DeserializeBaseUniformData(
		const YAML::Node& baseNode, 
		std::vector<ShaderDataUniform>& outDataUniforms, 
		std::vector<ShaderTextureUniform>& outTextureUniforms)
	{
		YAML::Node dataUniformsNode = baseNode["data uniforms"];

		for (YAML::const_iterator it = dataUniformsNode.begin(); it != dataUniformsNode.end(); ++it)
		{
			outDataUniforms.emplace_back(
				(*it)["name"].as<string>(),
				static_cast<ShaderStageFlags>((*it)["binding points"].as<int>()),
				static_cast<BufferDataType>((*it)["type"].as<int>())
			);
		}

		YAML::Node textureUniformsNode = baseNode["texture uniforms"];

		for (YAML::const_iterator it = textureUniformsNode.begin(); it != textureUniformsNode.end(); ++it)
		{
			outTextureUniforms.emplace_back(
				(*it)["name"].as<string>(),
				static_cast<ShaderStageFlags>((*it)["binding points"].as<int>()),
				static_cast<ShaderTextureUniform::DefaultTextureType>((*it)["default"].as<int>())
			);
		}
	}
}