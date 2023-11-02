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

	string ShaderSerializer::Serialize(SharedRef<Resource> resource)
	{
		SharedRef<Shader> shader = std::dynamic_pointer_cast<Shader>(resource);
		Assert(shader)

		YAML::Emitter out;
		out << YAML::BeginMap;

		out << YAML::Key << "name" << YAML::Value << shader->GetName();
		out << YAML::Key << "group tag" << YAML::Value << shader->GetGroupTag();
		out << YAML::Key << "pass shaders" << YAML::Value << YAML::BeginSeq;

		for (const ShaderVariant& variant : shader->GetShaderVariants())
		{
			SerializeShaderVariant(out, variant);
		}

		out << YAML::EndSeq;

		// HACK: the yaml parser throws an unknown character exception unless we add something to the end
		out << YAML::EndMap << YAML::Comment("Fix");

		Assert(out.good())

		return string(out.c_str());
	}

	SharedRef<Resource> ShaderSerializer::Deserialize(const std::type_index& type, const ResourceID& resourceID, const string& data)
	{
		YAML::Node shaderNode = YAML::Load(data);

		string name = shaderNode["name"].as<string>();
		string groupTag = shaderNode["group tag"].as<string>();

		SharedRef<Shader> shader = CreateSharedRef<Shader>(resourceID, name, groupTag);

		YAML::Node passShadersNode = shaderNode["pass shaders"];
		for (YAML::const_iterator it = passShadersNode.begin(); it != passShadersNode.end(); ++it)
		{
			shader->AddVariant(DeserializeShaderVariant(*it));
		}

		return shader;
	}

	void ShaderSerializer::SerializeShaderVariant(YAML::Emitter& emitter, const ShaderVariant& variant)
	{
		emitter << YAML::BeginMap;

		emitter << YAML::Key << "name" << YAML::Value << variant.Name;

		emitter << YAML::Key << "stages" << YAML::Value << YAML::BeginSeq;

		for (const ShaderStage& stage : variant.Stages)
		{
			emitter << YAML::BeginMap;

			emitter << YAML::Key << "type" << YAML::Value << static_cast<int>(stage.Type);
			emitter << YAML::Key << "entry name" << YAML::Value << stage.EntryPointName;
			emitter << YAML::Key << "file path" << YAML::Value << stage.SourceFilePath;

			emitter << YAML::EndMap;
		}

		emitter << YAML::EndSeq;

		emitter << YAML::Key << "pipeline state" << YAML::Value << YAML::BeginMap;

		emitter << YAML::Key << "topology" << YAML::Value << static_cast<int>(variant.PipelineState.TopologyMode);
		emitter << YAML::Key << "cull" << YAML::Value << static_cast<int>(variant.PipelineState.CullingMode);
		emitter << YAML::Key << "winding" << YAML::Value << static_cast<int>(variant.PipelineState.WindingMode);
		emitter << YAML::Key << "fill" << YAML::Value << static_cast<int>(variant.PipelineState.PolygonFillMode);
		emitter << YAML::Key << "depth clamping" << YAML::Value << variant.PipelineState.EnableDepthClamping;
		emitter << YAML::Key << "depth test" << YAML::Value << static_cast<int>(variant.PipelineState.DepthTestingMode);
		emitter << YAML::Key << "depth write" << YAML::Value << variant.PipelineState.EnableDepthWrite;

		emitter << YAML::EndMap;

		emitter << YAML::Key << "blend states" << YAML::Value << YAML::BeginSeq;

		for (const BlendState& blendState : variant.AttachmentBlendStates)
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

		emitter << YAML::Key << "vertex attributes" << YAML::Value << static_cast<int>(variant.VertexFormat.AdditionalAttributes);

		if (variant.GlobalUniforms.Hash != ShaderUniformLayout::EmptyHash)
		{
			emitter << YAML::Key << "global layout" << YAML::Value << YAML::BeginMap;
			ShaderUniformLayoutSerialization::SerializeLayout(emitter, variant.GlobalUniforms);
			emitter << YAML::EndMap;
		}

		if (variant.InstanceUniforms.Hash != ShaderUniformLayout::EmptyHash)
		{
			emitter << YAML::Key << "instance layout" << YAML::Value << YAML::BeginMap;
			ShaderUniformLayoutSerialization::SerializeLayout(emitter, variant.InstanceUniforms);
			emitter << YAML::EndMap;
		}

		if (variant.DrawUniforms.Hash != ShaderUniformLayout::EmptyHash)
		{
			emitter << YAML::Key << "draw layout" << YAML::Value << YAML::BeginMap;
			ShaderUniformLayoutSerialization::SerializeLayout(emitter, variant.DrawUniforms);
			emitter << YAML::EndMap;
		}

		emitter << YAML::EndMap;
	}

	ShaderVariant ShaderSerializer::DeserializeShaderVariant(const YAML::Node& baseNode)
	{
		string name = baseNode["name"].as<string>();

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

		VertexDataFormat format(static_cast<VertexAttrFlags>(baseNode["vertex attributes"].as<int>()));

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

		return ShaderVariant(
			name,
			stages,
			pipelineState,
			blendStates,
			format,
			globalLayout,
			instanceLayout,
			drawLayout
		);
	}
}