#include "Renderpch.h"
#include "ShaderSerializer.h"

#include "../Shader.h"
#include "Types/ShaderUniformLayoutSerializer.h"

#include <yaml-cpp/yaml.h>

namespace Coco::Rendering
{
	bool ShaderSerializer::SupportsFileExtension(const string& extension) const
	{
		return extension == ".cshader";
	}

	bool ShaderSerializer::SupportsResourceType(const std::type_index& type) const
	{
		return type == typeid(Shader);
	}

	const std::type_index ShaderSerializer::GetResourceTypeForExtension(const string& extension) const
	{
		return typeid(Shader);
	}

	string ShaderSerializer::Serialize(Ref<Resource> resource)
	{
		const Shader* shader = dynamic_cast<const Shader*>(resource.Get());
		Assert(shader)

		YAML::Emitter out;
		out << YAML::BeginMap;

		out << YAML::Key << "name" << YAML::Value << shader->GetName();
		out << YAML::Key << "group tag" << YAML::Value << shader->GetGroupTag();
		out << YAML::Key << "pass shaders" << YAML::Value << YAML::BeginSeq;

		for (const RenderPassShader& passShader : shader->GetRenderPassShaders())
		{
			SerializeRenderPassShader(out, passShader);
		}

		out << YAML::EndSeq;

		// HACK: the yaml parser throws an unknown character exception unless we add something to the end
		out << YAML::EndMap << YAML::Comment("Fix");

		Assert(out.good())

		return string(out.c_str());
	}

	ManagedRef<Resource> ShaderSerializer::Deserialize(const std::type_index& type, const ResourceID& resourceID, const string& data)
	{
		YAML::Node shaderNode = YAML::Load(data);

		string name = shaderNode["name"].as<string>();
		string groupTag = shaderNode["group tag"].as<string>();

		ManagedRef<Shader> shader = CreateManagedRef<Shader>(resourceID, name, groupTag);

		YAML::Node passShadersNode = shaderNode["pass shaders"];
		for (YAML::const_iterator it = passShadersNode.begin(); it != passShadersNode.end(); ++it)
		{
			shader->AddRenderPassShader(DeserializeRenderPassShader(*it));
		}

		return shader;
	}

	void ShaderSerializer::SerializeRenderPassShader(YAML::Emitter& emitter, const RenderPassShader& passShader)
	{
		emitter << YAML::BeginMap;

		emitter << YAML::Key << "id" << YAML::Value << passShader.ID;
		emitter << YAML::Key << "pass name" << YAML::Value << passShader.PassName;

		emitter << YAML::Key << "stages" << YAML::Value << YAML::BeginSeq;

		for (const ShaderStage& stage : passShader.Stages)
		{
			emitter << YAML::BeginMap;

			emitter << YAML::Key << "type" << YAML::Value << static_cast<int>(stage.Type);
			emitter << YAML::Key << "entry name" << YAML::Value << stage.EntryPointName;
			emitter << YAML::Key << "file path" << YAML::Value << stage.FilePath;

			emitter << YAML::EndMap;
		}

		emitter << YAML::EndSeq;

		emitter << YAML::Key << "pipeline state" << YAML::Value << YAML::BeginMap;

		emitter << YAML::Key << "topology" << YAML::Value << static_cast<int>(passShader.PipelineState.TopologyMode);
		emitter << YAML::Key << "cull" << YAML::Value << static_cast<int>(passShader.PipelineState.CullingMode);
		emitter << YAML::Key << "winding" << YAML::Value << static_cast<int>(passShader.PipelineState.WindingMode);
		emitter << YAML::Key << "fill" << YAML::Value << static_cast<int>(passShader.PipelineState.PolygonFillMode);
		emitter << YAML::Key << "depth clamping" << YAML::Value << passShader.PipelineState.EnableDepthClamping;
		emitter << YAML::Key << "depth test" << YAML::Value << static_cast<int>(passShader.PipelineState.DepthTestingMode);
		emitter << YAML::Key << "depth write" << YAML::Value << passShader.PipelineState.EnableDepthWrite;

		emitter << YAML::EndMap;

		emitter << YAML::Key << "blend states" << YAML::Value << YAML::BeginSeq;

		for (const BlendState& blendState : passShader.AttachmentBlendStates)
		{
			emitter << YAML::BeginMap;

			emitter << YAML::Key << "color src factor" << YAML::Value << static_cast<int>(blendState.ColorSourceFactor);
			emitter << YAML::Key << "color dst factor" << YAML::Value << static_cast<int>(blendState.ColorDestinationFactor);
			emitter << YAML::Key << "color blend op" << YAML::Value << static_cast<int>(blendState.ColorBlendOperation);
			emitter << YAML::Key << "alpha src factor" << YAML::Value << static_cast<int>(blendState.AlphaSourceFactor);
			emitter << YAML::Key << "alpha dst factor" << YAML::Value << static_cast<int>(blendState.AlphaDestinationFactor);
			emitter << YAML::Key << "alpha blend op" << YAML::Value << static_cast<int>(blendState.AlphaBlendOperation);

			emitter << YAML::EndMap;
		}

		emitter << YAML::EndSeq;

		emitter << YAML::Key << "vertex attributes" << YAML::Value << YAML::BeginSeq;

		for (const ShaderVertexAttribute& attr : passShader.VertexAttributes)
		{
			emitter << YAML::BeginMap;

			emitter << YAML::Key << "name" << YAML::Value << attr.Name;
			emitter << YAML::Key << "type" << YAML::Value << static_cast<int>(attr.Type);

			emitter << YAML::EndMap;
		}

		emitter << YAML::EndSeq;

		if (passShader.GlobalUniforms.Hash != ShaderUniformLayout::EmptyHash)
		{
			emitter << YAML::Key << "global layout" << YAML::Value << YAML::BeginMap;
			ShaderUniformLayoutSerialization::SerializeLayout(emitter, passShader.GlobalUniforms);
			emitter << YAML::EndMap;
		}

		if (passShader.InstanceUniforms.Hash != ShaderUniformLayout::EmptyHash)
		{
			emitter << YAML::Key << "instance layout" << YAML::Value << YAML::BeginMap;
			ShaderUniformLayoutSerialization::SerializeLayout(emitter, passShader.InstanceUniforms);
			emitter << YAML::EndMap;
		}

		if (passShader.DrawUniforms.Hash != ShaderUniformLayout::EmptyHash)
		{
			emitter << YAML::Key << "draw layout" << YAML::Value << YAML::BeginMap;
			ShaderUniformLayoutSerialization::SerializeLayout(emitter, passShader.DrawUniforms);
			emitter << YAML::EndMap;
		}

		emitter << YAML::EndMap;
	}

	RenderPassShader ShaderSerializer::DeserializeRenderPassShader(const YAML::Node& baseNode)
	{
		uint64 id = baseNode["id"].as<uint64>();
		string passName = baseNode["pass name"].as<string>();

		std::vector<ShaderStage> stages;

		YAML::Node stagesNode = baseNode["stages"];
		for (YAML::const_iterator it = stagesNode.begin(); it != stagesNode.end(); ++it)
		{
			stages.emplace_back(
				(*it)["entry name"].as<string>(),
				static_cast<ShaderStageType>((*it)["type"].as<int>()),
				(*it)["file path"].as<string>()
			);
		}

		YAML::Node pipelineStateNode = baseNode["pipeline state"];
		GraphicsPipelineState pipelineState{};
		pipelineState.TopologyMode = static_cast<TopologyMode>(pipelineStateNode["topology"].as<int>());
		pipelineState.CullingMode = static_cast<CullMode>(pipelineStateNode["cull"].as<int>());
		pipelineState.WindingMode = static_cast<TriangleWindingMode>(pipelineStateNode["winding"].as<int>());
		pipelineState.PolygonFillMode = static_cast<PolygonFillMode>(pipelineStateNode["fill"].as<int>());
		pipelineState.EnableDepthClamping = pipelineStateNode["depth clamping"].as<bool>();
		pipelineState.DepthTestingMode = static_cast<DepthTestingMode>(pipelineStateNode["depth test"].as<int>());
		pipelineState.EnableDepthWrite = pipelineStateNode["depth write"].as<bool>();

		std::vector<BlendState> blendStates;
		YAML::Node blendStatesNode = baseNode["blend states"];

		for (YAML::const_iterator it = blendStatesNode.begin(); it != blendStatesNode.end(); ++it)
		{
			blendStates.emplace_back(
				static_cast<BlendFactorMode>((*it)["color src factor"].as<int>()),
				static_cast<BlendFactorMode>((*it)["color dst factor"].as<int>()),
				static_cast<BlendOperation>((*it)["color blend op"].as<int>()),
				static_cast<BlendFactorMode>((*it)["alpha src factor"].as<int>()),
				static_cast<BlendFactorMode>((*it)["alpha dst factor"].as<int>()),
				static_cast<BlendOperation>((*it)["alpha blend op"].as<int>())
			);
		}

		std::vector<ShaderVertexAttribute> vertexAttrs;
		YAML::Node vertexAttrsNode = baseNode["vertex attributes"];

		for (YAML::const_iterator it = vertexAttrsNode.begin(); it != vertexAttrsNode.end(); ++it)
		{
			vertexAttrs.emplace_back(
				(*it)["name"].as<string>(),
				static_cast<BufferDataType>((*it)["type"].as<int>())
			);
		}

		GlobalShaderUniformLayout globalLayout{};

		if (baseNode["global layout"])
		{
			globalLayout = ShaderUniformLayoutSerialization::DeserializeGlobalLayout(baseNode["global layout"]);
		}

		ShaderUniformLayout instanceLayout{};

		if (baseNode["instance layout"])
		{
			instanceLayout = ShaderUniformLayoutSerialization::DeserializeLayout(baseNode["instance layout"]);
		}

		ShaderUniformLayout drawLayout{};

		if (baseNode["draw layout"])
		{
			drawLayout = ShaderUniformLayoutSerialization::DeserializeLayout(baseNode["draw layout"]);
		}

		return RenderPassShader(
			id,
			passName,
			stages,
			pipelineState,
			blendStates,
			vertexAttrs,
			globalLayout,
			instanceLayout,
			drawLayout
		);
	}
}