#include "HelloTriangleRenderPass.h"
#include <Coco/Rendering/Graphics/GraphicsPipelineState.h>
#include <Coco/Core/Engine.h>
#include <Coco/Rendering/Graphics/RenderView.h>

using namespace Coco;
using namespace Coco::Rendering;

List<AttachmentDescription> HelloTriangleRenderPass::GetAttachmentDescriptions() noexcept
{
    List<AttachmentDescription> attachments;
    attachments.Add(AttachmentDescription(PixelFormat::RGBA8, ColorSpace::sRGB, false));
    attachments.Add(AttachmentDescription(PixelFormat::Depth32_Stencil8, ColorSpace::Unknown, false));
    return attachments;
}

void HelloTriangleRenderPass::Execute(RenderContext& renderContext)
{
    const Ref<RenderView>& renderView = renderContext.GetRenderView();

    for (const ObjectRenderData& objectData : renderView->Objects)
    {
        const MeshRenderData& meshData = renderView->Meshs.at(objectData.MeshData);

        for (uint i = 0; i < meshData.Submeshes.Count(); i++)
        {
            const ResourceID& materialID = objectData.MaterialDatas[i];

            if (materialID == Resource::InvalidID)
                continue;

            const MaterialRenderData& materialData = renderView->Materials.at(materialID);
            const ShaderRenderData& shaderData = renderView->Shaders.at(materialData.ShaderID);

            if (shaderData.GroupTag.empty())
            {
                renderContext.UseMaterial(materialData.ID);

                renderContext.SetShaderMatrix4x4(ShaderDescriptorScope::Global, "_Projection", renderView->Projection);
                renderContext.SetShaderMatrix4x4(ShaderDescriptorScope::Global, "_View", renderView->View);
                renderContext.SetShaderVector3(ShaderDescriptorScope::Global, "_ViewPosition", renderView->ViewPosition);
                renderContext.SetShaderColor(ShaderDescriptorScope::Global, "_AmbientColor", _ambientColor);
                renderContext.SetShaderInt(ShaderDescriptorScope::Global, "_RenderMode", static_cast<int>(_renderMode));

                renderContext.SetShaderMatrix4x4(ShaderDescriptorScope::Draw, "_Model", objectData.ModelMatrix);

                renderContext.Draw(objectData, i);
            }
        }    
    }
}

void HelloTriangleRenderPass::SetRenderMode(RenderModeType mode)
{
    _renderMode = mode;
}
