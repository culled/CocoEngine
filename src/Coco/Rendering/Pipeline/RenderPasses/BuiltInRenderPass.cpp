#include "Renderpch.h"
#include "BuiltInRenderPass.h"

#include "../../Resources/BuiltinShaders.h"
#include "DefaultRenderPassFunctions.h"

namespace Coco::Rendering
{
	const uint64 BuiltInRenderPass::sLitVisibilityGroup = 1 << 1;
	const uint64 BuiltInRenderPass::sUnlitVisibilityGroup = 1 << 2;

	const std::array<AttachmentFormat, 2> BuiltInRenderPass::_sAttachments = {
		AttachmentFormat(ImagePixelFormat::RGBA8, ImageColorSpace::sRGB),
		AttachmentFormat(ImagePixelFormat::Depth32_Stencil8, ImageColorSpace::Linear)
	};

	SharedRef<Shader> BuiltInRenderPass::_sLitShader = nullptr;
	SharedRef<Shader> BuiltInRenderPass::_sUnlitShader = nullptr;

	BuiltInRenderPass::BuiltInRenderPass(bool useFrustumCulling) :
		_frustumCulling(useFrustumCulling)
	{
		if (!_sLitShader || !_sUnlitShader)
			CreateShaders();
	}

	void BuiltInRenderPass::Prepare(RenderContext& context, const RenderView& renderView)
	{
		DefaultRenderPassFunctions::ApplyDefaultPreparations(context, renderView);
	}

	void BuiltInRenderPass::Execute(RenderContext& context, const RenderView& renderView)
	{
		std::vector<uint64> objectIndices = renderView.GetRenderObjectIndices();

		if(_frustumCulling)
			renderView.FilterOutsideFrustum(objectIndices);

		RenderUnlitObjects(context, renderView, objectIndices);
		RenderLitObjects(context, renderView, objectIndices);
	}

	void BuiltInRenderPass::SetUseFrustumCulling(bool useFrustumCulling)
	{
		_frustumCulling = useFrustumCulling;
	}

	void BuiltInRenderPass::CreateShaders()
	{
		_sLitShader = BuiltInShaders::GetLitShader();
		_sUnlitShader = BuiltInShaders::GetUnlitShader();
	}

	void BuiltInRenderPass::RenderUnlitObjects(RenderContext& context, const RenderView& renderView, const std::vector<uint64>& objectIndices)
	{
		std::vector<uint64> unlitObjectIndices = objectIndices;
		renderView.FilterWithAnyVisibilityGroups(unlitObjectIndices, sUnlitVisibilityGroup);

		RenderObjects(context, renderView, unlitObjectIndices, _sUnlitShader->GetName());
	}

	void BuiltInRenderPass::RenderLitObjects(RenderContext& context, const RenderView& renderView, const std::vector<uint64>& objectIndices)
	{
		std::vector<uint64> litObjectIndices = objectIndices;
		renderView.FilterWithAnyVisibilityGroups(litObjectIndices, sLitVisibilityGroup);

		RenderObjects(context, renderView, litObjectIndices, _sLitShader->GetName());
	}

	void BuiltInRenderPass::RenderObjects(RenderContext& context, const RenderView& renderView, const std::vector<uint64>& objectIndices, const string& shaderName)
	{
		context.SetShader(shaderName);

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