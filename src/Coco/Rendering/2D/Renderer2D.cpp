//
// Created by cullen on 4/11/26.
//
#if 0
#include "Renderer2D.h"

#include "Coco/Core/Math/Quaternion.h"
#include "Coco/Rendering/MeshUtils.h"
#include "Coco/Rendering/Shader.h"
#include "Coco/Core/Engine.h"

namespace Coco
{
    Render2DDrawCall::Render2DDrawCall(uint32 indexOffset, SharedPtr<Texture> drawTexture) :
        IndexOffset(indexOffset),
        DrawTexture(std::move(drawTexture))
    {}

    void Render2DBatch::ClearData()
    {
        Positions.Clear();
        Colors.Clear();
        UVs.Clear();
        Indices.Clear();
        DrawCalls.Clear();
    }

    Renderer2D::Renderer2D() :
        _batches(),
        _currentBatch(nullptr),
        _currentRotation(Quaternion::Identity)
    {}

    Renderer2D::~Renderer2D() noexcept
    {
        _batches.Clear();
    }

    void Renderer2D::SetCurrentRenderTarget(uint64 targetID)
    {
        _currentRotation = Quaternion::Identity;

        _currentBatch = _batches.TryGetValue(targetID);

        if (!_currentBatch)
            _currentBatch = &_batches.Emplace(targetID);
    }

    void Renderer2D::SetRotation(float rotation)
    {
        _currentRotation = Quaternion(Vector3(0.0f, 0.0f, rotation));
    }

    void Renderer2D::ClearRotation()
    {
        _currentRotation = Quaternion::Identity;
    }

    void Renderer2D::Draw(const Rect& rect, SharedPtr<Texture> drawTexture, const Color& tintColor)
    {
        if (!_currentBatch)
            return;

        float halfWidth = rect.Size.Width * 0.5f;
        float halfHeight = rect.Size.Height * 0.5f;
        uint32 vertexOffset = static_cast<uint32>(_currentBatch->Positions.GetCount());
        Vector3 offset(rect.Offset, 0.0f);

        StackArray<Vector3, 4> positions = {
            _currentRotation * Vector3(-halfWidth, -halfHeight, 0.0f) + offset,
            _currentRotation * Vector3(-halfWidth, halfHeight, 0.0f) + offset,
            _currentRotation * Vector3(halfWidth, halfHeight, 0.0f) + offset,
            _currentRotation * Vector3(halfWidth, -halfHeight, 0.0f) + offset,
        };

        _currentBatch->Positions.AppendRange(positions);

        Vector4 c = tintColor.AsVector4(false);
        _currentBatch->Colors.Append(c);
        _currentBatch->Colors.Append(c);
        _currentBatch->Colors.Append(c);
        _currentBatch->Colors.Append(c);

        StackArray<Vector2, 4> uvs = {
            Vector2(0.0f, 0.0f),
            Vector2(0.0f, 1.0f),
            Vector2(1.0f, 1.0f),
            Vector2(1.0f, 0.0f),
        };

        _currentBatch->UVs.AppendRange(uvs);

        uint32 indexOffset = static_cast<uint32>(_currentBatch->Indices.GetCount());

        uint32 indices[] = {
            vertexOffset,
            vertexOffset + 1,
            vertexOffset + 3,
            vertexOffset + 2,
            vertexOffset + 3,
            vertexOffset + 1,
        };

        _currentBatch->Indices.AppendRange(indices);

        _currentBatch->DrawCalls.EmplaceBack(indexOffset, drawTexture);
    }

    void Renderer2D::Reset()
    {
        for (auto& pair : _batches)
            pair.second.ClearData();

        _currentBatch = nullptr;
    }

    void Renderer2D::Render(uint64 targetID, RenderGraph& graph, RenderScene& scene)
    {
        if (_batches.IsEmpty())
            return;

        if (!_shader2D)
            _shader2D = Engine::Get()->GetResourceManager()->CreateResource<Shader>("2D Shader", "Shaders/BuiltIn/2D.slang");

        if (auto batch = _batches.TryGetValue(targetID))
        {
            uint64 meshID = Math::CombineHashes(targetID, typeid(Renderer2D).hash_code());
            scene.AddMeshData(meshID, batch->Positions, batch->Indices, {}, batch->Colors, {}, batch->UVs);

            for (uint64 i = 0; i < batch->DrawCalls.GetCount(); i++)
            {
                const auto& drawCall = batch->DrawCalls[i];
                uint64 id = Math::CombineHashes(meshID, i);
                scene.StoreData(id, false, drawCall);
                scene.AddObject(id, 0, meshID, drawCall.IndexOffset, 6);
            }
        }

        struct Render2DData
        {
            SharedPtr<Shader> Shader2D;
            RenderGraphResourceRef ColorOutput;
            Matrix4x4 Projection;
        };

        graph.CreateRenderPass<Render2DData>("Render 2D",
        [&](RenderGraphBuilder& builder, Render2DData& data)
        {
            data.Shader2D = _shader2D;
            data.ColorOutput = builder.WriteRenderTarget(0);
            auto size = graph.GetAttachmentSize();
            data.Projection = scene.CreateOrthographicProjection(0.0f, static_cast<float>(size.Width), 0.0f, static_cast<float>(size.Height), -10.0f, 10.0f);
        },
        [](const Render2DData& data, const RenderScene& sceneData, RenderContext& ctx)
        {
            GraphicsPipelineState pipelineState;
            pipelineState.CullingMode = CullMode::None;
            pipelineState.BlendState = AttachmentBlendState::AlphaBlending;
            pipelineState.EnableDepthWrite = false;

            ctx.SetShader(*data.Shader2D, pipelineState);

            ShaderCursor globalCursor;
            if (ctx.CreateAndBindGlobalBuffer("cameraData2D", globalCursor))
            {
                globalCursor.Field("Projection").Write(data.Projection);
            }

            for (const auto& obj : sceneData.GetRenderObjectView())
            {
                if (sceneData.HasData<Render2DDrawCall>(obj.ID, false))
                {
                    const Render2DDrawCall* drawCall = sceneData.GetData<Render2DDrawCall>(obj.ID, false);
                    ctx.SetDrawData(nullptr, 0, Span<const SharedPtr<Texture>>({drawCall->DrawTexture}));

                    ctx.DrawObject(obj);
                }
            }
        });
    }
} // Coco
#endif
