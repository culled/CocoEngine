#include "Renderpch.h"
#include "BuiltInShaders.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	ResourceID BuiltInShaders::_litShaderID = Resource::InvalidID;
	ResourceID BuiltInShaders::_unlitShaderID = Resource::InvalidID;

	SharedRef<Shader> BuiltInShaders::GetLitShader()
	{
		auto& resources = Engine::Get()->GetResourceLibrary();

		if (_litShaderID == Resource::InvalidID || !resources.Has(_litShaderID))
			return CreateLitShader();

		return resources.GetAs<Shader>(_litShaderID);
	}

	SharedRef<Shader> BuiltInShaders::GetUnlitShader()
	{
		auto& resources = Engine::Get()->GetResourceLibrary();

		if (_unlitShaderID == Resource::InvalidID || !resources.Has(_unlitShaderID))
			return CreateUnlitShader();

		return resources.GetAs<Shader>(_unlitShaderID);
	}

	SharedRef<Shader> BuiltInShaders::CreateLitShader()
	{
		SharedRef<Shader> shader = Engine::Get()->GetResourceLibrary().Create<Shader>(
			"DefaultLit",
			std::vector<ShaderStage>({
				ShaderStage("main", ShaderStageType::Vertex, "shaders/built-in/Lit.vert.glsl"),
				ShaderStage("main", ShaderStageType::Fragment, "shaders/built-in/Lit.frag.glsl")
			}),
			GraphicsPipelineState(),
			std::vector<BlendState>({
				BlendState::Opaque
			}),
			VertexDataFormat(VertexAttrFlags::Normal | VertexAttrFlags::Tangent | VertexAttrFlags::UV0),
			GlobalShaderUniformLayout(),
			ShaderUniformLayout(
				{
					ShaderUniform("AlbedoTintColor", ShaderUniformType::Color, ShaderStageFlags::Fragment, Color::White),
					ShaderUniform("AlbedoTexture", ShaderUniformType::Texture, ShaderStageFlags::Fragment, DefaultTextureType::White),
					ShaderUniform("NormalTexture", ShaderUniformType::Texture, ShaderStageFlags::Fragment, DefaultTextureType::FlatNormal)
				}
			),
			ShaderUniformLayout(
				{
					ShaderUniform("ModelMatrix", ShaderUniformType::Mat4x4, ShaderStageFlags::Vertex, Matrix4x4::Identity)
				}
		));

		_litShaderID = shader->GetID();

		return shader;
	}

	SharedRef<Shader> BuiltInShaders::CreateUnlitShader()
	{
		SharedRef<Shader> shader = Engine::Get()->GetResourceLibrary().Create<Shader>(
			"DefaultUnlit",
			std::vector<ShaderStage>({
				ShaderStage("main", ShaderStageType::Vertex, "shaders/built-in/Unlit.vert.glsl"),
				ShaderStage("main", ShaderStageType::Fragment, "shaders/built-in/Unlit.frag.glsl")
			}),
			GraphicsPipelineState(),
			std::vector<BlendState>({
				BlendState::Opaque
			}),
			VertexDataFormat(VertexAttrFlags::UV0),
			GlobalShaderUniformLayout(),
			ShaderUniformLayout(
				{
					ShaderUniform("TintColor", ShaderUniformType::Color, ShaderStageFlags::Fragment, Color::White),
					ShaderUniform("ColorTexture", ShaderUniformType::Texture, ShaderStageFlags::Fragment, DefaultTextureType::White)
				}
			),
			ShaderUniformLayout(
				{
					ShaderUniform("ModelMatrix", ShaderUniformType::Mat4x4, ShaderStageFlags::Vertex, Matrix4x4::Identity)
				}
			)
		);

		_unlitShaderID = shader->GetID();

		return shader;
	}
}