#include "Renderpch.h"
#include "BuiltInShaders.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	ResourceID BuiltInShaders::_litShaderID = ResourceID::Invalid;
	ResourceID BuiltInShaders::_unlitShaderID = ResourceID::Invalid;

	SharedRef<Shader> BuiltInShaders::GetLitShader()
	{
		auto& resources = Engine::Get()->GetResourceLibrary();

		if (_litShaderID == ResourceID::Invalid || !resources.Has(_litShaderID))
			return CreateLitShader();

		return resources.GetAs<Shader>(_litShaderID);
	}

	SharedRef<Shader> BuiltInShaders::GetUnlitShader()
	{
		auto& resources = Engine::Get()->GetResourceLibrary();

		if (_unlitShaderID == ResourceID::Invalid || !resources.Has(_unlitShaderID))
			return CreateUnlitShader();

		return resources.GetAs<Shader>(_unlitShaderID);
	}

	SharedRef<Shader> BuiltInShaders::CreateLitShader()
	{
		SharedRef<Shader> shader = Engine::Get()->GetResourceLibrary().Create<Shader>(
			ResourceID("BuiltInShaders::Lit"),
			"BuiltIn-Lit",
			std::initializer_list<ShaderStage>({
				ShaderStage("main", ShaderStageType::Vertex, "shaders/built-in/Lit.vert.glsl"),
				ShaderStage("main", ShaderStageType::Fragment, "shaders/built-in/Lit.frag.glsl")
				}),
			GraphicsPipelineState(),
			std::initializer_list<AttachmentBlendState>({
				AttachmentBlendState::Opaque
				}),
			VertexDataFormat(VertexAttrFlags::Normal | VertexAttrFlags::Tangent | VertexAttrFlags::UV0),
			ShaderUniformLayout::Empty,
			ShaderUniformLayout(
				{
					ShaderUniform("AlbedoTintColor", ShaderStageFlags::Fragment, Color::White),
					ShaderUniform("AlbedoTexture", ShaderStageFlags::Fragment, DefaultTextureType::White),
					ShaderUniform("NormalTexture", ShaderStageFlags::Fragment, DefaultTextureType::FlatNormal)
				}
			),
			ShaderUniformLayout(
				{
					ShaderUniform("ModelMatrix", ShaderStageFlags::Vertex, Matrix4x4::Identity)
				}
		));

		_litShaderID = shader->GetID();

		return shader;
	}

	SharedRef<Shader> BuiltInShaders::CreateUnlitShader()
	{
		SharedRef<Shader> shader = Engine::Get()->GetResourceLibrary().Create<Shader>(
			ResourceID("BuiltInShaders::Unlit"),
			"BuiltIn-Unlit",
			std::initializer_list<ShaderStage>({
				ShaderStage("main", ShaderStageType::Vertex, "shaders/built-in/Unlit.vert.glsl"),
				ShaderStage("main", ShaderStageType::Fragment, "shaders/built-in/Unlit.frag.glsl")
				}),
			GraphicsPipelineState(),
			std::initializer_list<AttachmentBlendState>({
				AttachmentBlendState::Opaque
				}),
			VertexDataFormat(VertexAttrFlags::UV0),
			ShaderUniformLayout::Empty,
			ShaderUniformLayout(
				{
					ShaderUniform("TintColor", ShaderStageFlags::Fragment, Color::White),
					ShaderUniform("ColorTexture", ShaderStageFlags::Fragment, DefaultTextureType::White)
				}
			),
			ShaderUniformLayout(
				{
					ShaderUniform("ModelMatrix", ShaderStageFlags::Vertex, Matrix4x4::Identity)
				}
			)
		);

		_unlitShaderID = shader->GetID();

		return shader;
	}
}