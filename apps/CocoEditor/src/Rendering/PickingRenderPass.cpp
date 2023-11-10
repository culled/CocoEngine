#include "PickingRenderPass.h"

#include <Coco/Rendering/Pipeline/RenderPasses/DefaultRenderPassFunctions.h>
#include <Coco/Core/Engine.h>

using namespace Coco::Rendering;

namespace Coco
{
	const string PickingRenderPass::sName = "picking";

	std::vector<AttachmentFormat> PickingRenderPass::_sAttachments = {
		AttachmentFormat(ImagePixelFormat::R32_Int, ImageColorSpace::Linear, true),
		AttachmentFormat(ImagePixelFormat::Depth32_Stencil8, ImageColorSpace::Linear, false)
	};

	SharedRef<Shader> PickingRenderPass::_sShader = nullptr;

	PickingRenderPass::PickingRenderPass()
	{
		if (!_sShader)
			CreateShader();
	}

	void PickingRenderPass::Prepare(RenderContext& context, const RenderView& renderView)
	{
		DefaultRenderPassFunctions::ApplyDefaultPreparations(context, renderView);
	}

	void PickingRenderPass::Execute(RenderContext& context, const RenderView& renderView)
	{
		std::vector<uint64> objectIndices = renderView.GetRenderObjectIndices();
		renderView.FilterOutsideFrustum(objectIndices);

		context.SetShader(_sShader->GetName());

		for (const uint64& i : objectIndices)
		{
			const ObjectData& obj = renderView.GetRenderObject(i);
			const MeshData& mesh = renderView.GetMeshData(obj.MeshID);

			context.SetValue(UniformScope::Draw, ShaderUniformData::MakeKey("ModelMatrix"), obj.ModelMatrix);
			context.SetValue(UniformScope::Draw, ShaderUniformData::MakeKey("ID"), static_cast<int>(obj.ObjectID));
			context.DrawIndexed(mesh, obj.IndexOffset, obj.IndexCount);
		}
	}

	void PickingRenderPass::CreateShader()
	{
		_sShader = Engine::Get()->GetResourceLibrary().Create<Shader>(
			PickingRenderPass::sName,
			std::vector<ShaderStage>({
				ShaderStage("main", ShaderStageType::Vertex, "shaders/built-in/Picking.vert.glsl"),
				ShaderStage("main", ShaderStageType::Fragment, "shaders/built-in/Picking.frag.glsl"),
			}),
			GraphicsPipelineState(),
			std::vector<BlendState>({
				BlendState::Opaque
			}),
			VertexDataFormat(),
			GlobalShaderUniformLayout(),
			ShaderUniformLayout(),
			ShaderUniformLayout(
				{
					ShaderUniform("ModelMatrix", ShaderUniformType::Mat4x4, ShaderStageFlags::Vertex, Matrix4x4::Identity),
					ShaderUniform("ID", ShaderUniformType::Int, ShaderStageFlags::Vertex, -1)
				}
			)
		);
	}
}