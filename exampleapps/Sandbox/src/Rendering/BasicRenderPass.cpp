#include "BasicRenderPass.h"

std::vector<AttachmentFormat> BasicRenderPass::GetInputAttachments() const
{
    return std::vector<AttachmentFormat>({
        AttachmentFormat(ImagePixelFormat::RGBA8, false)
        });
}

void BasicRenderPass::Execute(RenderContext& context, const RenderView& renderView)
{
    for (const ObjectData& obj : renderView.GetRenderObjects())
    {
        const ShaderData& shader = renderView.GetShaderData(obj.ShaderID);

        auto it = shader.RenderPassShaders.find(GetName());
        if (it == shader.RenderPassShaders.end())
            continue;

        context.SetShader(renderView.GetRenderPassShaderData(it->second));

        context.SetMatrix4x4(UniformScope::Global, ShaderUniformData::MakeKey("projectionMatrix"), renderView.GetProjectionMatrix());
        context.SetMatrix4x4(UniformScope::Global, ShaderUniformData::MakeKey("viewMatrix"), renderView.GetViewMatrix());
        context.SetFloat4(UniformScope::Instance, ShaderUniformData::MakeKey("baseColor"), Color::Green);
        context.SetMatrix4x4(UniformScope::Draw, ShaderUniformData::MakeKey("modelMatrix"), obj.ModelMatrix);
        context.Draw(renderView.GetMeshData(obj.MeshID));
    }
}
