#include "Renderpch.h"
#include "BasicShaderRenderPass.h"

#include "DefaultRenderPassFunctions.h"

namespace Coco::Rendering
{
	const std::array<AttachmentFormat, 2> BasicShaderRenderPass::_sAttachments = {
		AttachmentFormat(ImagePixelFormat::RGBA8, ImageColorSpace::sRGB),
		AttachmentFormat(ImagePixelFormat::Depth32_Stencil8, ImageColorSpace::Linear)
	};

	BasicShaderRenderPass::BasicShaderRenderPass(SharedRef<Shader> shader, uint64 visibilityGroups, bool exclusiveVisibilityGroups, bool useFrustumCulling) :
		_shader(shader),
		_passName(shader->GetName()),
		_visibilityGroups(visibilityGroups),
		_exclusiveVisibilityGroups(exclusiveVisibilityGroups),
		_useFrustumCulling(useFrustumCulling)
	{}

	void BasicShaderRenderPass::Prepare(RenderContext& context, const RenderView& renderView)
	{
		DefaultRenderPassFunctions::ApplyDefaultPreparations(context, renderView);
	}

	void BasicShaderRenderPass::Execute(RenderContext& context, const RenderView& renderView)
	{
		std::vector<uint64> objectIndices = renderView.GetRenderObjectIndices();

		if (_exclusiveVisibilityGroups)
			renderView.FilterWithAllVisibilityGroups(objectIndices, _visibilityGroups);
		else
			renderView.FilterWithAnyVisibilityGroups(objectIndices, _visibilityGroups);

		if (_useFrustumCulling)
			renderView.FilterOutsideFrustum(objectIndices);

		for (const uint64& i : objectIndices)
		{
			const ObjectData& obj = renderView.GetRenderObject(i);
			const MeshData& mesh = renderView.GetMeshData(obj.MeshID);

			if (obj.MaterialID != RenderView::InvalidID)
			{
				const MaterialData& material = renderView.GetMaterialData(obj.MaterialID);

				context.SetMaterial(material);
			}
			else
			{
				context.ClearInstanceProperties();
			}

			context.SetValue(UniformScope::Draw, ShaderUniformData::MakeKey("ModelMatrix"), obj.ModelMatrix);
			context.DrawIndexed(mesh, obj.IndexOffset, obj.IndexCount);
		}
	}
}