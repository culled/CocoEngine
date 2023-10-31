#include "Renderpch.h"
#include "BuiltInShaders.h"

namespace Coco::Rendering
{
	const ShaderVariant BuiltInShaders::LitVariant = ShaderVariant(
		"DefaultLit",
		{
			ShaderStage("main", ShaderStageType::Vertex, "shaders/built-in/Lit.vert.glsl"),
			ShaderStage("main", ShaderStageType::Fragment, "shaders/built-in/Lit.frag.glsl")
		},
		GraphicsPipelineState(),
		{
			BlendState::Opaque
		},
		{
			ShaderVertexAttribute("Position", BufferDataType::Float3),
			ShaderVertexAttribute("Normal", BufferDataType::Float3),
			ShaderVertexAttribute("Tangent", BufferDataType::Float4),
			ShaderVertexAttribute("UV", BufferDataType::Float2)
		},
		GlobalShaderUniformLayout(),
		ShaderUniformLayout(
			{
				ShaderDataUniform("AlbedoTintColor", ShaderStageFlags::Fragment, BufferDataType::Float4)
			},
			{
				ShaderTextureUniform("AlbedoTexture", ShaderStageFlags::Fragment, ShaderTextureUniform::DefaultTextureType::White),
				ShaderTextureUniform("NormalTexture", ShaderStageFlags::Fragment, ShaderTextureUniform::DefaultTextureType::Normal)
			}
		),
		ShaderUniformLayout(
			{
				ShaderDataUniform("ModelMatrix", ShaderStageFlags::Vertex, BufferDataType::Mat4x4)
			},
			{}
		)
	);

	const ShaderVariant BuiltInShaders::UnlitVariant = ShaderVariant(
		"DefaultUnlit",
		{
			ShaderStage("main", ShaderStageType::Vertex, "shaders/built-in/Unlit.vert.glsl"),
			ShaderStage("main", ShaderStageType::Fragment, "shaders/built-in/Unlit.frag.glsl")
		},
		GraphicsPipelineState(),
		{
			BlendState::Opaque
		},
		{
			ShaderVertexAttribute("Position", BufferDataType::Float3),
			ShaderVertexAttribute("UV", BufferDataType::Float2)
		},
		GlobalShaderUniformLayout(),
		ShaderUniformLayout(
			{
				ShaderDataUniform("TintColor", ShaderStageFlags::Fragment, BufferDataType::Float4)
			},
			{
				ShaderTextureUniform("ColorTexture", ShaderStageFlags::Fragment, ShaderTextureUniform::DefaultTextureType::Normal)
			}
		),
		ShaderUniformLayout(
			{
				ShaderDataUniform("ModelMatrix", ShaderStageFlags::Vertex, BufferDataType::Mat4x4)
			},
			{}
		)
	);
}