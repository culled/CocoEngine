#include "PickingRenderPass.h"

#include <Coco/Rendering/Pipeline/RenderPasses/DefaultRenderPassFunctions.h>

using namespace Coco::Rendering;

namespace Coco
{
	const string PickingRenderPass::sName = "picking";

	std::vector<AttachmentFormat> PickingRenderPass::_sAttachments = {
		AttachmentFormat(ImagePixelFormat::R32_Int, ImageColorSpace::Linear, AttachmentClearMode::Clear, true),
		AttachmentFormat(ImagePixelFormat::Depth32_Stencil8, ImageColorSpace::Linear, AttachmentClearMode::Clear, false)
	};

	void PickingRenderPass::Prepare(RenderContext& context, const RenderView& renderView)
	{
		DefaultRenderPassFunctions::ApplyDefaultPreparations(context, renderView);
	}

	void PickingRenderPass::Execute(RenderContext& context, const RenderView& renderView)
	{
		std::vector<uint64> objectIndices = renderView.GetRenderObjectIndices();
		renderView.FilterWithShaderVariant(objectIndices, sName);
		renderView.FilterOutsideFrustum(objectIndices);

		for (const uint64& i : objectIndices)
		{
			const ObjectData& obj = renderView.GetRenderObject(i);
			const MeshData& mesh = renderView.GetMeshData(obj.MeshID);
			const MaterialData& material = renderView.GetMaterialData(obj.MaterialID);

			context.SetMaterial(material, sName);
			context.SetMatrix4x4(UniformScope::Draw, ShaderUniformData::MakeKey("ModelMatrix"), obj.ModelMatrix);
			context.SetInt(UniformScope::Draw, ShaderUniformData::MakeKey("ID"), static_cast<int32>(obj.ObjectID));
			context.DrawIndexed(mesh, obj.IndexOffset, obj.IndexCount);
		}
	}
}