#include "ImGuiRenderPass.h"

#include "ImGuiCocoPlatform.h"
#include "ImGuiService.h"
#include "ImGuiSceneDataProvider.h"
#include <Coco/Rendering/Texture.h>
#include <Coco/Rendering/Mesh.h>
#include <Coco/Rendering/RenderService.h>
#include <Coco/Core/Engine.h>

//#include <imgui.h>

namespace Coco::ImGuiCoco
{
    const string ImGuiRenderPass::Name = "imgui";

    const uint64 ImGuiRenderPass::VisibilityGroup = static_cast<uint64>(1) << 62;

    const std::array<RenderPassAttachment, 1> ImGuiRenderPass::_attachments = {
        RenderPassAttachment(ImagePixelFormat::RGBA8, ImageColorSpace::sRGB, AttachmentUsageFlags::Color)
    };

    const string ImGuiRenderPass::_shaderName = "ImGui";

    SharedRef<Shader> ImGuiRenderPass::_shader = nullptr;

    ImGuiRenderPass::ImGuiRenderPass()
    {
        if (!_shader)
            CreateShader();
    }

    void ImGuiRenderPass::Execute(RenderContext& context, const RenderView& renderView)
    {
        ResourceLibrary& resources = Engine::Get()->GetResourceLibrary();

        std::vector<uint64> objectIndices = renderView.GetRenderObjectIndices();
        renderView.FilterWithAllVisibilityGroups(objectIndices, VisibilityGroup);

        context.SetShader(_shader);

        for (uint64 i : objectIndices)
        {
            const RenderObjectData& obj = renderView.GetRenderObject(i);

            if (!obj.ExtraData.has_value())
                continue;

            if (const ImGuiExtraData* extraData = std::any_cast<const ImGuiExtraData>(&obj.ExtraData))
            {
                SharedRef<Texture> tex;

                if (!resources.TryGetAs(extraData->OverrideTextureID, tex))
                {
                    CocoError("Texture resource for ImGui draw was null")
                    tex = RenderService::Get()->GetDefaultCheckerTexture();
                }

                std::array<ShaderUniformValue, 1> uniforms = { ShaderUniformValue("Texture", tex) };
                context.SetDrawUniforms(uniforms);
                context.SetScissorRect(extraData->ScissorRect);
            }
            else
            {
                CocoError("Extra object data was not a ImGuiExtraData object")
                continue;
            }

            context.Draw(obj.Mesh, obj.Submesh);
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

        _shader = Engine::Get()->GetResourceLibrary().Create<Shader>(
            ResourceID("ImGuiRenderPass::Shader"),
            _shaderName,
            std::initializer_list<ShaderStage>({
                ShaderStage("main", ShaderStageType::Vertex, "shaders/built-in/ImGui.vert.glsl"),
                ShaderStage("main", ShaderStageType::Fragment, "shaders/built-in/ImGui.frag.glsl")
                }),
            pipelineState,
            std::initializer_list<AttachmentBlendState>({
                AttachmentBlendState::AlphaBlending
                }),
            VertexDataFormat(VertexAttrFlags::Color | VertexAttrFlags::UV0),
            ShaderUniformLayout::Empty,
            ShaderUniformLayout::Empty,
            ShaderUniformLayout(
                {
                    ShaderUniform("Texture", ShaderStageFlags::Fragment, DefaultTextureType::White)
                }
            )
        );
    }
}