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

	string ShaderSerializer::Serialize(SharedRef<Resource> resource)
	{
		SharedRef<Shader> shader = std::dynamic_pointer_cast<Shader>(resource);
		Assert(shader)

		YAML::Emitter out;
		out << YAML::BeginMap;

		out << YAML::Key << "stages" << YAML::Value << YAML::BeginSeq;

		for (const ShaderStage& stage : shader->GetStages())
		{
			out << YAML::BeginMap;

			out << YAML::Key << "type" << YAML::Value << static_cast<int>(stage.Type);
			out << YAML::Key << "entry name" << YAML::Value << stage.EntryPointName;
			out << YAML::Key << "source file path" << YAML::Value << stage.SourceFilePath.ToString();

			out << YAML::EndMap;
		}

		out << YAML::EndSeq;

		out << YAML::Key << "pipeline state" << YAML::Value << YAML::BeginMap;

		const GraphicsPipelineState& pipelineState = shader->GetPipelineState();
		out << YAML::Key << "topology" << YAML::Value << static_cast<int>(pipelineState.TopologyMode);
		out << YAML::Key << "cull" << YAML::Value << static_cast<int>(pipelineState.CullingMode);
		out << YAML::Key << "winding" << YAML::Value << static_cast<int>(pipelineState.WindingMode);
		out << YAML::Key << "fill" << YAML::Value << static_cast<int>(pipelineState.PolygonFillMode);
		out << YAML::Key << "depth clamping" << YAML::Value << pipelineState.EnableDepthClamping;
		out << YAML::Key << "depth test" << YAML::Value << static_cast<int>(pipelineState.DepthTestingMode);
		out << YAML::Key << "depth write" << YAML::Value << pipelineState.EnableDepthWrite;

		out << YAML::EndMap;

		out << YAML::Key << "blend states" << YAML::Value << YAML::BeginSeq;

		for (const BlendState& blendState : shader->GetAttachmentBlendStates())
		{
			out << YAML::BeginMap;

			out << YAML::Key << "color src factor" << YAML::Value << static_cast<int>(blendState.ColorSourceFactor);
			out << YAML::Key << "color dst factor" << YAML::Value << static_cast<int>(blendState.ColorDestinationFactor);
			out << YAML::Key << "color blend op" << YAML::Value << static_cast<int>(blendState.ColorBlendOperation);
			out << YAML::Key << "alpha src factor" << YAML::Value << static_cast<int>(blendState.AlphaSourceFactor);
			out << YAML::Key << "alpha dst factor" << YAML::Value << static_cast<int>(blendState.AlphaDestinationFactor);
			out << YAML::Key << "alpha blend op" << YAML::Value << static_cast<int>(blendState.AlphaBlendOperation);

			out << YAML::EndMap;
		}

		out << YAML::EndSeq;

		const VertexDataFormat& vertexFormat = shader->GetVertexDataFormat();
		out << YAML::Key << "vertex attributes" << YAML::Value << static_cast<int>(vertexFormat.AdditionalAttributes);

		const GlobalShaderUniformLayout& globalLayout = shader->GetGlobalUniformLayout();
		if (globalLayout.Hash != ShaderUniformLayout::EmptyHash)
		{
			out << YAML::Key << "global layout" << YAML::Value << YAML::BeginMap;
			ShaderUniformLayoutSerialization::SerializeLayout(out, globalLayout);
			out << YAML::EndMap;
		}

		const ShaderUniformLayout& instanceLayout = shader->GetInstanceUniformLayout();
		if (instanceLayout.Hash != ShaderUniformLayout::EmptyHash)
		{
			out << YAML::Key << "instance layout" << YAML::Value << YAML::BeginMap;
			ShaderUniformLayoutSerialization::SerializeLayout(out, instanceLayout);
			out << YAML::EndMap;
		}

		const ShaderUniformLayout& drawLayout = shader->GetDrawUniformLayout();
		if (drawLayout.Hash != ShaderUniformLayout::EmptyHash)
		{
			out << YAML::Key << "draw layout" << YAML::Value << YAML::BeginMap;
			ShaderUniformLayoutSerialization::SerializeLayout(out, drawLayout);
			out << YAML::EndMap;
		}

		// HACK: the yaml parser throws an unknown character exception unless we add something to the end
		out << YAML::EndMap << YAML::Comment("Fix");

		Assert(out.good())

		return string(out.c_str());
	}

	SharedRef<Resource> ShaderSerializer::CreateAndDeserialize(const ResourceID& id, const string& name, const string& data)
	{
		SharedRef<Shader> shader = CreateSharedRef<Shader>(id, name);
		Deserialize(data, shader);

		return shader;
	}

	bool ShaderSerializer::Deserialize(const string& data, SharedRef<Resource> resource)
	{
		SharedRef<Shader> shader = std::dynamic_pointer_cast<Shader>(resource);

		Assert(shader)

		YAML::Node baseNode = YAML::Load(data);

		std::vector<ShaderStage> stages;

		YAML::Node stagesNode = baseNode["stages"];
		for (YAML::const_iterator it = stagesNode.begin(); it != stagesNode.end(); ++it)
		{
			stages.emplace_back(
				(*it)["entry name"].as<string>(),
				static_cast<ShaderStageType>((*it)["type"].as<int>()),
				(*it)["source file path"].as<string>()
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
			ShaderUniformLayoutSerialization::DeserializeGlobalLayout(baseNode["global layout"], globalLayout);
		}

		ShaderUniformLayout instanceLayout{};

		if (baseNode["instance layout"])
		{
			ShaderUniformLayoutSerialization::DeserializeLayout(baseNode["instance layout"], instanceLayout);
		}

		ShaderUniformLayout drawLayout{};

		if (baseNode["draw layout"])
		{
			ShaderUniformLayoutSerialization::DeserializeLayout(baseNode["draw layout"], drawLayout);
		}

		shader->SetStages(stages);
		shader->SetPipelineState(pipelineState);
		shader->SetAttachmentBlendStates(blendStates);
		shader->SetVertexDataFormat(format);
		shader->SetGlobalUniformLayout(globalLayout);
		shader->SetInstanceUniformLayout(instanceLayout);
		shader->SetDrawUniformLayout(drawLayout);
		shader->IncrementVersion();

		return true;
	}
}