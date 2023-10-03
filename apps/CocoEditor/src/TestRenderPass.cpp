#include "TestRenderPass.h"

namespace Coco
{
	std::vector<AttachmentFormat> TestRenderPass::_sAttachments = {
		AttachmentFormat(ImagePixelFormat::RGBA8, ImageColorSpace::sRGB, true)
	};

	void TestRenderPass::Prepare(RenderContext& context, const RenderView& renderView)
	{
		context.SetMatrix4x4(UniformScope::Global, ShaderUniformData::MakeKey("ProjectionMatrix"), renderView.GetProjectionMatrix());
		context.SetMatrix4x4(UniformScope::Global, ShaderUniformData::MakeKey("ViewMatrix"), renderView.GetViewMatrix());
	}

	void TestRenderPass::Execute(RenderContext& context, const RenderView& renderView)
	{
		for (const ObjectData& obj : renderView.GetRenderObjects())
		{
			const MaterialData& material = renderView.GetMaterialData(obj.MaterialID);
			const ShaderData& shader = renderView.GetShaderData(material.ShaderID);

			context.SetMaterial(material);

			context.SetMatrix4x4(UniformScope::Draw, ShaderUniformData::MakeKey("ModelMatrix"), obj.ModelMatrix);
			context.DrawIndexed(renderView.GetMeshData(obj.MeshID), obj.IndexOffset, obj.IndexCount);
		}
	}
}