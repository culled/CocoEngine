#include "Renderpch.h"
#include "BasicShaderRenderPass.h"

#include "../../Shader.h"

namespace Coco::Rendering
{
	BasicShaderRenderPass::BasicShaderRenderPass(
		SharedRef<Shader> shader, 
		std::span<const RenderPassAttachment> attachmentFormats, 
		uint64 visibilityGroups, 
		bool exclusiveVisibilityGroups, 
		bool useFrustumCulling) :
		_shader(shader),
		_passName(shader->GetName()),
		_attachments(attachmentFormats.begin(), attachmentFormats.end()),
		_visibilityGroups(visibilityGroups),
		_exclusiveVisibilityGroups(exclusiveVisibilityGroups),
		_useFrustumCulling(useFrustumCulling)
	{}

	void BasicShaderRenderPass::Execute(RenderContext& context, const RenderView& renderView)
	{
		std::vector<uint64> objectIndices = renderView.GetRenderObjectIndices();

		if (_exclusiveVisibilityGroups)
			renderView.FilterWithAllVisibilityGroups(objectIndices, _visibilityGroups);
		else
			renderView.FilterWithAnyVisibilityGroups(objectIndices, _visibilityGroups);

		if (_useFrustumCulling)
			renderView.FilterOutsideFrustum(objectIndices);

		context.SetShader(_shader);

		for (const uint64& i : objectIndices)
		{
			const RenderObjectData& obj = renderView.GetRenderObject(i);

			if (obj.Material)
				context.SetMaterial(obj.Material);

			std::array<ShaderUniformValue, 1> drawUniforms = { ShaderUniformValue("ModelMatrix", obj.ModelMatrix) };
			context.SetDrawUniforms(drawUniforms);

			context.Draw(obj.Mesh, obj.Submesh);
		}
	}
}