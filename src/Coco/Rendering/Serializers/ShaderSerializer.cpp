#include "Renderpch.h"
#include "ShaderSerializer.h"
#include "../Shader.h"
#include "../Texture.h"

#include <Coco/Core/Types/YAMLConverters.h>
#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	bool ShaderSerializer::SerializeYAML(YAML::Emitter& emitter, const SharedRef<Resource>& resource)
	{
		SharedRef<Shader> shader = std::dynamic_pointer_cast<Shader>(resource);

		if (!shader)
		{
			CocoError("Resource was not a shader resource")
			return false;
		}

		emitter << YAML::Key << "stages" << YAML::Value << YAML::BeginSeq;

		for (const ShaderStage& stage : shader->GetStages())
		{
			emitter << YAML::BeginMap;

			emitter << YAML::Key << "type" << YAML::Value << static_cast<int>(stage.Type);
			emitter << YAML::Key << "entryName" << YAML::Value << stage.EntryPointName;
			emitter << YAML::Key << "sourceFilePath" << YAML::Value << stage.SourceFilePath.ToString();

			emitter << YAML::EndMap;
		}

		emitter << YAML::EndSeq;

		emitter << YAML::Key << "pipelineState" << YAML::Value << YAML::BeginMap;

		const GraphicsPipelineState& pipelineState = shader->GetPipelineState();
		emitter << YAML::Key << "topology" << YAML::Value << static_cast<int>(pipelineState.TopologyMode);
		emitter << YAML::Key << "cull" << YAML::Value << static_cast<int>(pipelineState.CullingMode);
		emitter << YAML::Key << "winding" << YAML::Value << static_cast<int>(pipelineState.WindingMode);
		emitter << YAML::Key << "fill" << YAML::Value << static_cast<int>(pipelineState.PolygonFillMode);
		emitter << YAML::Key << "depthClamping" << YAML::Value << pipelineState.EnableDepthClamping;
		emitter << YAML::Key << "depthTest" << YAML::Value << static_cast<int>(pipelineState.DepthTestingMode);
		emitter << YAML::Key << "depthWrite" << YAML::Value << pipelineState.EnableDepthWrite;

		emitter << YAML::EndMap;

		emitter << YAML::Key << "blendStates" << YAML::Value << YAML::BeginSeq;

		for (const AttachmentBlendState& blendState : shader->GetAttachmentBlendStates())
		{
			emitter << YAML::BeginMap;

			emitter << YAML::Key << "colorSrcFactor" << YAML::Value << static_cast<int>(blendState.ColorSourceFactor);
			emitter << YAML::Key << "colorDstFactor" << YAML::Value << static_cast<int>(blendState.ColorDestinationFactor);
			emitter << YAML::Key << "colorBlendOp" << YAML::Value << static_cast<int>(blendState.ColorBlendOperation);
			emitter << YAML::Key << "alphaSrcFactor" << YAML::Value << static_cast<int>(blendState.AlphaSourceFactor);
			emitter << YAML::Key << "alphaDstFactor" << YAML::Value << static_cast<int>(blendState.AlphaDestinationFactor);
			emitter << YAML::Key << "alphaBlendOp" << YAML::Value << static_cast<int>(blendState.AlphaBlendOperation);

			emitter << YAML::EndMap;
		}

		emitter << YAML::EndSeq;

		const VertexDataFormat& vertexFormat = shader->GetVertexDataFormat();
		emitter << YAML::Key << "vertexAttributes" << YAML::Value << static_cast<int>(vertexFormat.AdditionalAttributes);

		const ShaderUniformLayout& globalLayout = shader->GetGlobalUniformLayout();
		if (globalLayout != ShaderUniformLayout::Empty)
		{
			emitter << YAML::Key << "globalLayout" << YAML::Value << YAML::BeginMap;
			SerializeUniformLayout(emitter, globalLayout);
			emitter << YAML::EndMap;
		}

		const ShaderUniformLayout& instanceLayout = shader->GetInstanceUniformLayout();
		if (instanceLayout != ShaderUniformLayout::Empty)
		{
			emitter << YAML::Key << "instanceLayout" << YAML::Value << YAML::BeginMap;
			SerializeUniformLayout(emitter, instanceLayout);
			emitter << YAML::EndMap;
		}

		const ShaderUniformLayout& drawLayout = shader->GetDrawUniformLayout();
		if (drawLayout != ShaderUniformLayout::Empty)
		{
			emitter << YAML::Key << "drawLayout" << YAML::Value << YAML::BeginMap;
			SerializeUniformLayout(emitter, drawLayout);
			emitter << YAML::EndMap;
		}

		return true;
	}

	SharedRef<Resource> ShaderSerializer::CreateResource(const ResourceID& id)
	{
		return CreateSharedRef<Shader>(id, "Shader");
	}

	bool ShaderSerializer::DeserializeYAML(const YAML::Node& baseNode, SharedRef<Resource> resource)
	{
		SharedRef<Shader> shader = std::dynamic_pointer_cast<Shader>(resource);

		if (!shader)
		{
			CocoError("Resource was not a shader resource")
			return false;
		}


		YAML::Node stagesNode = baseNode["stages"];
		shader->_stages.clear();
		for (YAML::const_iterator it = stagesNode.begin(); it != stagesNode.end(); ++it)
		{
			shader->_stages.emplace_back(
				(*it)["entryName"].as<string>(),
				static_cast<ShaderStageType>((*it)["type"].as<int>()),
				(*it)["sourceFilePath"].as<string>()
			);
		}

		YAML::Node pipelineStateNode = baseNode["pipelineState"];
		shader->_pipelineState.TopologyMode = static_cast<TopologyMode>(pipelineStateNode["topology"].as<int>());
		shader->_pipelineState.CullingMode = static_cast<CullMode>(pipelineStateNode["cull"].as<int>());
		shader->_pipelineState.WindingMode = static_cast<TriangleWindingMode>(pipelineStateNode["winding"].as<int>());
		shader->_pipelineState.PolygonFillMode = static_cast<PolygonFillMode>(pipelineStateNode["fill"].as<int>());
		shader->_pipelineState.EnableDepthClamping = pipelineStateNode["depthClamping"].as<bool>();
		shader->_pipelineState.DepthTestingMode = static_cast<DepthTestingMode>(pipelineStateNode["depthTest"].as<int>());
		shader->_pipelineState.EnableDepthWrite = pipelineStateNode["depthWrite"].as<bool>();

		YAML::Node blendStatesNode = baseNode["blendStates"];
		shader->_attachmentBlendStates.clear();
		for (YAML::const_iterator it = blendStatesNode.begin(); it != blendStatesNode.end(); ++it)
		{
			shader->_attachmentBlendStates.emplace_back(
				static_cast<BlendFactorMode>((*it)["colorSrcFactor"].as<int>()),
				static_cast<BlendFactorMode>((*it)["colorDstFactor"].as<int>()),
				static_cast<BlendOperation>((*it)["colorBlendOp"].as<int>()),
				static_cast<BlendFactorMode>((*it)["alphaSrcFactor"].as<int>()),
				static_cast<BlendFactorMode>((*it)["alphaDstFactor"].as<int>()),
				static_cast<BlendOperation>((*it)["alphaBlendOp"].as<int>())
			);
		}

		shader->_vertexFormat = VertexDataFormat(static_cast<VertexAttrFlags>(baseNode["vertex attributes"].as<int>()));

		if (baseNode["globalLayout"])
			shader->_globalUniforms = DeserializeUniformLayout(baseNode["globalLayout"]);

		if (baseNode["instanceLayout"])
			shader->_instanceUniforms = DeserializeUniformLayout(baseNode["instanceLayout"]);

		if (baseNode["drawLayout"])
			shader->_drawUniforms = DeserializeUniformLayout(baseNode["drawLayout"]);

		shader->IncrementVersion();

		return true;
	}

	void ShaderSerializer::SerializeUniformLayout(YAML::Emitter& emitter, const ShaderUniformLayout& layout)
	{
		emitter << YAML::Key << "uniforms" << YAML::Value << YAML::BeginSeq;

		for (const ShaderUniform* u : layout.GetUniforms(true, true))
		{
			emitter << YAML::BeginMap;

			emitter << YAML::Key << "name" << YAML::Value << u->Name;
			emitter << YAML::Key << "type" << YAML::Value << static_cast<int>(u->Type);
			emitter << YAML::Key << "bindingPoints" << YAML::Value << static_cast<int>(u->BindingPoints);
			emitter << YAML::Key << "defaultValue" << YAML::Value;
			
			switch (u->Type)
			{
				case ShaderUniformType::Float:
				emitter << std::any_cast<float>(u->DefaultValue);
				break;
			case ShaderUniformType::Float2:
				emitter << static_cast<std::span<const float>>(std::any_cast<Float2>(u->DefaultValue));
				break;
			case ShaderUniformType::Float3:
				emitter << static_cast<std::span<const float>>(std::any_cast<Float3>(u->DefaultValue));
				break;
			case ShaderUniformType::Float4:
				emitter << static_cast<std::span<const float>>(std::any_cast<Float4>(u->DefaultValue));
				break;
			case ShaderUniformType::Color:
			{		
				Float4 values = std::any_cast<Float4>(u->DefaultValue);
				Color c(values, true);
				emitter << c;
				break;
			}
			case ShaderUniformType::Matrix4x4:
				emitter << static_cast<std::span<const float>>(std::any_cast<FloatMatrix4x4>(u->DefaultValue));
				break;
			case ShaderUniformType::Int:
				emitter << std::any_cast<int>(u->DefaultValue);
				break;
			case ShaderUniformType::Int2:
				emitter << static_cast<std::span<const int>>(std::any_cast<Int2>(u->DefaultValue));
				break;
			case ShaderUniformType::Int3:
				emitter << static_cast<std::span<const int>>(std::any_cast<Int3>(u->DefaultValue));
				break;
			case ShaderUniformType::Int4:
				emitter << static_cast<std::span<const int>>(std::any_cast<Int4>(u->DefaultValue));
				break;
			case ShaderUniformType::Bool:
				emitter << std::any_cast<bool>(u->DefaultValue);
				break;
			case ShaderUniformType::Texture:
			{
				if (const SharedRef<Texture>* tex = std::any_cast<const SharedRef<Texture>>(&u->DefaultValue))
				{
					if (*tex)
					{
						emitter << *(*tex);
					}
					else
					{
						emitter << "";
					}
				}
				else if (const DefaultTextureType* texType = std::any_cast<const DefaultTextureType>(&u->DefaultValue))
				{
					emitter << FormatString(":{}", static_cast<int>(*texType));
				}
				else
				{
					emitter << "";
					CocoWarn("Default texture parameter \"{}\" is not a known type", u->Name)
				}

				break;
			}
			default:
				CocoWarn("Parameter \"{}\" is not a known ShaderUniform type", u->Name)
				break;
			}

			emitter << YAML::EndMap;
		}

		emitter << YAML::EndSeq;
	}

	ShaderUniformLayout ShaderSerializer::DeserializeUniformLayout(const YAML::Node& baseNode)
	{
		std::vector<ShaderUniform> uniforms;
		YAML::Node dataUniformsNode = baseNode["uniforms"];

		for (YAML::const_iterator it = dataUniformsNode.begin(); it != dataUniformsNode.end(); ++it)
		{
			string name = (*it)["string"].as<string>();
			ShaderUniformType type = static_cast<ShaderUniformType>((*it)["type"].as<int>());
			ShaderStageFlags bindingPoints = static_cast<ShaderStageFlags>((*it)["bindingPoints"].as<int>());
			std::any defaultValue;

			YAML::Node defaultNode = (*it)["defaultValue"];

			switch (type)
			{
			case ShaderUniformType::Float:
				uniforms.emplace_back(name, bindingPoints, defaultNode.as<float>());
				break;
			case ShaderUniformType::Float2:
				uniforms.emplace_back(name, bindingPoints, defaultNode.as<Vector2>());
				break;
			case ShaderUniformType::Float3:
				uniforms.emplace_back(name, bindingPoints, defaultNode.as<Vector3>());
				break;
			case ShaderUniformType::Float4:
				uniforms.emplace_back(name, bindingPoints, defaultNode.as<Vector4>());
				break;
			case ShaderUniformType::Color:
				uniforms.emplace_back(name, bindingPoints, defaultNode.as<Color>());
				break;
			case ShaderUniformType::Matrix4x4:		
				uniforms.emplace_back(name, bindingPoints, defaultNode.as<Matrix4x4>());
				break;
			case ShaderUniformType::Int:
				uniforms.emplace_back(name, bindingPoints, defaultNode.as<int>());
				break;
			case ShaderUniformType::Int2:
				uniforms.emplace_back(name, bindingPoints, defaultNode.as<Vector2Int>());
				break;
			case ShaderUniformType::Int3:
				uniforms.emplace_back(name, bindingPoints, defaultNode.as<Vector3Int>());
				break;
			case ShaderUniformType::Int4:
				uniforms.emplace_back(name, bindingPoints, defaultNode.as<Vector4Int>());
				break;
			case ShaderUniformType::Bool:
				uniforms.emplace_back(name, bindingPoints, defaultNode.as<bool>());
				break;
			case ShaderUniformType::Texture:
			{
				string value = defaultNode.as<string>();

				if (value[0] == ':')
				{
					string valueStr = value.substr(1);
					uniforms.emplace_back(name, bindingPoints, static_cast<DefaultTextureType>(atoi(valueStr.c_str())));
				}
				else
				{
					SharedRef<Texture> tex = std::dynamic_pointer_cast<Texture>(LoadResourceFromYAML(defaultNode));

					if (!tex)
					{
						CocoWarn("Could not load texture for default texture parameter \"{}\"", name)
					}

					uniforms.emplace_back(name, bindingPoints, tex);
				}
				break;
			}
			}
		}

		return ShaderUniformLayout(uniforms);
	}
}