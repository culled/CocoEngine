#include "ImGuiRenderPass.h"

#include "ImGuiCocoPlatform.h"
#include "ImGuiService.h"
#include <Coco/Rendering/Texture.h>
#include <Coco/Core/Engine.h>
#include <imgui.h>

namespace Coco::ImGuiCoco
{
    const string ImGuiRenderPass::sPassName = "imgui";
    const uint64 ImGuiRenderPass::_sVisibilityGroup = static_cast<uint64>(1) << 62;
    const string ImGuiRenderPass::_sShaderName = "ImGui";
    SharedRef<Shader> ImGuiRenderPass::_sShader = nullptr;

    ImGuiRenderPass::ImGuiRenderPass() :
        _attachments({ AttachmentFormat(ImagePixelFormat::RGBA8, ImageColorSpace::sRGB) })
    {
        if (!_sShader)
            CreateShader();
    }

    void ImGuiRenderPass::Prepare(RenderContext& context, const RenderView& renderView)
    {
        context.SetValue(UniformScope::Global, ShaderUniformData::MakeKey("ProjectionMatrix"), renderView.GetProjectionMatrix());
    }

    void ImGuiRenderPass::Execute(RenderContext& context, const RenderView& renderView)
    {
        std::vector<uint64> objectIndices = renderView.GetRenderObjectIndices();
        renderView.FilterWithAllVisibilityGroups(objectIndices, _sVisibilityGroup);

        context.SetShader(_sShaderName);

        for (uint64 i : objectIndices)
        {
            const ObjectData& obj = renderView.GetRenderObject(i);

            if (obj.ExtraData.has_value())
            {
                ShaderUniformData::TextureSampler sampler = std::any_cast<ShaderUniformData::TextureSampler>(obj.ExtraData);

                context.SetTextureSampler(UniformScope::Draw, ShaderUniformData::MakeKey("Texture"), sampler.first, sampler.second);
            }

            const MeshData& mesh = renderView.GetMeshData(obj.MeshID);
            context.SetScissorRect(obj.ScissorRect);
            context.DrawIndexed(mesh, obj.IndexOffset, obj.IndexCount);
        }
    }

    void ImGuiRenderPass::CreateShader()
    {
        // Setup ImGui shader
        GraphicsPipelineState pipelineState{};
        pipelineState.WindingMode = TriangleWindingMode::CounterClockwise;
        pipelineState.CullingMode = CullMode::None;
        pipelineState.DepthTestingMode = DepthTestingMode::Never;
        pipelineState.EnableDepthWrite = false;

        Engine::Get()->GetResourceLibrary().Create<Shader>(
            _sShaderName,
            std::vector<ShaderStage>({
                ShaderStage("main", ShaderStageType::Vertex, "shaders/built-in/ImGui.vert.glsl"),
                ShaderStage("main", ShaderStageType::Fragment, "shaders/built-in/ImGui.frag.glsl")
                }),
            pipelineState,
            std::vector<BlendState>({
                BlendState::AlphaBlending
                }),
            VertexDataFormat(VertexAttrFlags::Color | VertexAttrFlags::UV0),
            GlobalShaderUniformLayout(),
            ShaderUniformLayout(),
            ShaderUniformLayout(
                {
                    ShaderUniform("Texture", ShaderUniformType::Texture, ShaderStageFlags::Fragment, DefaultTextureType::White)
                }
            )

        );
    }
}