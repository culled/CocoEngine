//
// Created by cullen on 4/11/26.
//

#if 0
#ifndef COCOENGINE_RENDERER2D_H
#define COCOENGINE_RENDERER2D_H
#include "Coco/Core/Math/Matrix4x4.h"
#include "Coco/Core/Math/Rect.h"
#include "Coco/Core/Memory/Ptrs.h"
#include "Coco/Core/Types/Array.h"
#include "Coco/Core/Types/Map.h"
#include "Coco/Core/Types/Color.h"
#include "Coco/Rendering/RenderListener.h"

namespace Coco
{
    class Texture;
    class Mesh;

    struct Render2DDrawCall
    {
        uint32 IndexOffset;
        SharedPtr<Texture> DrawTexture;

        Render2DDrawCall(uint32 indexOffset, SharedPtr<Texture> drawTexture);
    };

    struct Render2DBatch
    {
        Array<Vector3> Positions;
        Array<Vector4> Colors;
        Array<Vector2> UVs;
        Array<uint32> Indices;
        Array<Render2DDrawCall> DrawCalls;

        void ClearData();
    };

    class Renderer2D
    {
    public:
        Renderer2D();
        ~Renderer2D() noexcept;

        void SetCurrentRenderTarget(uint64 targetID);
        void SetRotation(float rotation);
        void ClearRotation();

        void Draw(const Rect& rect, SharedPtr<Texture> drawTexture, const Color& tintColor = Color::White);
        void Reset();

        void Render(uint64 targetID, RenderGraph& graph, RenderScene& scene);

    private:
        Map<uint64, Render2DBatch> _batches;
        Render2DBatch* _currentBatch;
        Quaternion _currentRotation;
        SharedPtr<Shader> _shader2D;
    };
} // Coco

#endif //COCOENGINE_RENDERER2D_H
#endif