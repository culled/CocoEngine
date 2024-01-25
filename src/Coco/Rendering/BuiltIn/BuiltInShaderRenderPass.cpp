#include "Renderpch.h"
#include "BuiltInShaderRenderPass.h"

#include "BuiltInShaders.h"

namespace Coco::Rendering
{
	const uint64 BuiltInShaderRenderPass::LitVisibilityGroup = 1 << 1;
	const uint64 BuiltInShaderRenderPass::UnlitVisibilityGroup = 1 << 2;
	const string BuiltInShaderRenderPass::_name = "Built-In";

	const std::array<RenderPassAttachment, 2> BuiltInShaderRenderPass::_attachments = {
		RenderPassAttachment(ImagePixelFormat::RGBA8, ImageColorSpace::sRGB, AttachmentUsageFlags::Color),
		RenderPassAttachment(ImagePixelFormat::Depth32_Stencil8, ImageColorSpace::Linear, AttachmentUsageFlags::Depth)
	};

	BuiltInShaderRenderPass::BuiltInShaderRenderPass(bool useFrustumCulling) :
		_frustumCulling(useFrustumCulling),
		_litShader(BuiltInShaders::GetLitShader()),
		_unlitShader(BuiltInShaders::GetUnlitShader())
	{}

	void BuiltInShaderRenderPass::Execute(RenderContext& context, const RenderView& renderView)
	{
		std::vector<uint64> objectIndices = renderView.GetRenderObjectIndices();

		if (_frustumCulling)
			renderView.FilterOutsideFrustum(objectIndices);

		RenderUnlitObjects(context, renderView, objectIndices);
		RenderLitObjects(context, renderView, objectIndices);
	}

	void BuiltInShaderRenderPass::SetUseFrustumCulling(bool useFrustumCulling)
	{
		_frustumCulling = useFrustumCulling;
	}

	void BuiltInShaderRenderPass::RenderUnlitObjects(RenderContext& context, const RenderView& renderView, const std::vector<uint64>& objectIndices)
	{
		std::vector<uint64> unlitObjectIndices = objectIndices;
		renderView.FilterWithAnyVisibilityGroups(unlitObjectIndices, UnlitVisibilityGroup);

		if(unlitObjectIndices.size() > 0)
			RenderObjects(context, renderView, unlitObjectIndices, _unlitShader);
	}

	void BuiltInShaderRenderPass::RenderLitObjects(RenderContext& context, const RenderView& renderView, const std::vector<uint64>& objectIndices)
	{
		std::vector<uint64> litObjectIndices = objectIndices;
		renderView.FilterWithAnyVisibilityGroups(litObjectIndices, LitVisibilityGroup);

		if(litObjectIndices.size() > 0)
			RenderObjects(context, renderView, litObjectIndices, _litShader);
	}

	void BuiltInShaderRenderPass::RenderObjects(RenderContext& context, const RenderView& renderView, const std::vector<uint64>& objectIndices, const SharedRef<Shader>& shader)
	{
		context.SetShader(shader);

		for (const uint64& i : objectIndices)
		{
			const RenderObjectData& obj = renderView.GetRenderObject(i);

			context.SetMaterial(obj.Material);

			std::array<ShaderUniformValue, 1> drawUniforms = { ShaderUniformValue("ModelMatrix", obj.ModelMatrix) };
			context.SetDrawUniforms(drawUniforms);

			context.Draw(obj.Mesh, obj.Submesh);
		}
	}
}