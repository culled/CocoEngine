#include "TestRenderPass.h"
#include <Coco/Core/Engine.h>

using namespace Coco::Rendering;

const string TestRenderPass::_name = "Test";
const std::array<RenderPassAttachment, 1> TestRenderPass::_attachments = {
	RenderPassAttachment(ImagePixelFormat::RGBA8, ImageColorSpace::sRGB, AttachmentUsageFlags::Color)
};

TestRenderPass::TestRenderPass()
{
	ResourceLibrary& resources = Engine::Get()->GetResourceLibrary();

	GraphicsPipelineState pipelineState{};
	pipelineState.CullingMode = CullMode::Back;

	_uniformTestShader = resources.Create<Shader>(
		ResourceID("TestRenderPass::Shader"),
		"UniformTest",
		std::initializer_list<ShaderStage>{
			ShaderStage("main", ShaderStageType::Vertex, "shaders/testing/UniformTest.vert.glsl"),
			ShaderStage("main", ShaderStageType::Fragment, "shaders/testing/UniformTest.frag.glsl")
		},
		pipelineState,
		std::initializer_list<AttachmentBlendState>{
		AttachmentBlendState::Opaque
		},
		VertexDataFormat(),
		ShaderUniformLayout({
			ShaderUniform("TestShaderGlobal", ShaderStageFlags::Fragment, 0.0f)
			}),
		ShaderUniformLayout({
			ShaderUniform("TestShaderInstance", ShaderStageFlags::Fragment, 0.0f)
			}),
		ShaderUniformLayout({
			ShaderUniform("TestDraw", ShaderStageFlags::Vertex, Vector3::Zero)
			})
	);

	_textureTestShader = resources.Create<Shader>(
		ResourceID("TestRenderPass::TextureShader"),
		"TextureTest",
		std::initializer_list<ShaderStage>{
			ShaderStage("main", ShaderStageType::Vertex, "shaders/testing/TextureTest.vert.glsl"),
			ShaderStage("main", ShaderStageType::Fragment, "shaders/testing/TextureTest.frag.glsl")
		},
		pipelineState,
		std::initializer_list<AttachmentBlendState>{
			AttachmentBlendState::Opaque
		},
		VertexDataFormat(),
		ShaderUniformLayout::Empty,
		ShaderUniformLayout({
			ShaderUniform("TestTexture", ShaderStageFlags::Fragment, DefaultTextureType::Checkered)
			}),
		ShaderUniformLayout::Empty
	);
}

void TestRenderPass::Execute(RenderContext& context, const RenderView& renderView)
{
	DrawUniformTest(context, renderView);
	DrawTextureTest(context, renderView);
}

void TestRenderPass::DrawUniformTest(Rendering::RenderContext& context, const Rendering::RenderView& renderView)
{
	double time = MainLoop::Get()->GetCurrentTick().Time;
	double t = Math::Cos(time * 2.0) * 0.5 + 0.5;
	double t2 = Math::Cos(time * 4.0) * 0.5 + 0.5;

	context.SetShader(_uniformTestShader);

	std::array<ShaderUniformValue, 1> globalUniforms = {
		ShaderUniformValue("TestShaderGlobal", static_cast<float>(t))
	};

	context.SetGlobalShaderUniforms(globalUniforms);

	std::array<ShaderUniformValue, 1> drawUniforms = {
		ShaderUniformValue("TestDraw", Vector3(Math::Cos(time) * 0.5, Math::Sin(time) * 0.5, 0.0))
	};

	for (const auto& obj : renderView.GetRenderObjects())
	{
		if (!obj.Mesh)
			continue;

		if (obj.Material)
			context.SetMaterial(obj.Material);

		context.SetDrawUniforms(drawUniforms);
		context.Draw(obj.Mesh, obj.Submesh);
	}
}

void TestRenderPass::DrawTextureTest(Rendering::RenderContext& context, const Rendering::RenderView& renderView)
{
	context.SetShader(_textureTestShader);

	for (const auto& obj : renderView.GetRenderObjects())
	{
		if (!obj.Mesh)
			continue;

		if (obj.Material)
			context.SetMaterial(obj.Material);

		context.Draw(obj.Mesh, obj.Submesh);
	}
}
