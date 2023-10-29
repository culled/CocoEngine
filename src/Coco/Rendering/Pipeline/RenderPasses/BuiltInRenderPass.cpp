#include "Renderpch.h"
#include "BuiltInRenderPass.h"

#include "../../Resources/BuiltinShaders.h"
#include "DefaultRenderPassFunctions.h"

namespace Coco::Rendering
{
	const string BuiltInRenderPass::sGroupTag = "";

	const std::array<AttachmentFormat, 2> BuiltInRenderPass::_sAttachments = {
		AttachmentFormat(ImagePixelFormat::RGBA8, ImageColorSpace::sRGB, AttachmentClearMode::Clear, true),
		AttachmentFormat(ImagePixelFormat::Depth32_Stencil8, ImageColorSpace::Linear, AttachmentClearMode::Clear, true)
	};

	BuiltInRenderPass::BuiltInRenderPass(bool useFrustumCulling) :
		_frustumCulling(useFrustumCulling)
	{}

	void BuiltInRenderPass::Prepare(RenderContext& context, const RenderView& renderView)
	{
		DefaultRenderPassFunctions::ApplyDefaultPreparations(context, renderView);
	}

	void BuiltInRenderPass::Execute(RenderContext& context, const RenderView& renderView)
	{
		std::vector<uint64> objectIndices = renderView.GetRenderObjectIndices();

		renderView.FilterWithTag(objectIndices, sGroupTag);

		if(_frustumCulling)
			renderView.FilterOutsideFrustum(objectIndices);

		RenderUnlitObjects(context, renderView, objectIndices);
		RenderLitObjects(context, renderView, objectIndices);
	}

	void BuiltInRenderPass::SetUseFrustumCulling(bool useFrustumCulling)
	{
		_frustumCulling = useFrustumCulling;
	}

	void BuiltInRenderPass::RenderUnlitObjects(RenderContext& context, const RenderView& renderView, const std::vector<uint64>& objectIndices)
	{
		std::vector<uint64> unlitObjectIndices = objectIndices;
		renderView.FilterWithShaderVariant(unlitObjectIndices, BuiltInShaders::UnlitVariant.Name);

		RenderObjects(context, renderView, unlitObjectIndices, BuiltInShaders::UnlitVariant.Name);
	}

	void BuiltInRenderPass::RenderLitObjects(RenderContext& context, const RenderView& renderView, const std::vector<uint64>& objectIndices)
	{
		std::vector<uint64> litObjectIndices = objectIndices;
		renderView.FilterWithShaderVariant(litObjectIndices, BuiltInShaders::LitVariant.Name);

		RenderObjects(context, renderView, litObjectIndices, BuiltInShaders::LitVariant.Name);
	}

	void BuiltInRenderPass::RenderObjects(RenderContext& context, const RenderView& renderView, const std::vector<uint64>& objectIndices, const string& shaderVariantName)
	{
		for (const uint64& i : objectIndices)
		{
			const ObjectData& obj = renderView.GetRenderObject(i);
			const MeshData& mesh = renderView.GetMeshData(obj.MeshID);
			const MaterialData& material = renderView.GetMaterialData(obj.MaterialID);

			context.SetMaterial(material, shaderVariantName);
			context.SetMatrix4x4(UniformScope::Draw, ShaderUniformData::MakeKey("ModelMatrix"), obj.ModelMatrix);
			context.DrawIndexed(mesh, obj.IndexOffset, obj.IndexCount);
		}
	}
}