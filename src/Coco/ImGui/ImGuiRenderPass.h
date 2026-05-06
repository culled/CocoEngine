//
// Created by cullen on 4/6/26.
//

#ifndef COCOENGINE_IMGUIRENDERPASS_H
#define COCOENGINE_IMGUIRENDERPASS_H
#include "Coco/Core/Math/Matrix4x4.h"
#include "Coco/Core/Math/Rect.h"
#include "Coco/Core/Memory/Ptrs.h"
#include "Coco/Rendering/RenderGraph/RenderGraphTypes.h"

namespace Coco
{
    class Shader;

    struct ImGuiObjectData
    {
        Recti ScissorRect;
        uint64 TextureID;
    };

    struct ImGuiRenderPassData
    {
        SharedPtr<Shader> ImGuiShader;
        RenderGraphResourceRef ColorOutput;
        Matrix4x4 Projection;
    };
} // Coco

#endif //COCOENGINE_IMGUIRENDERPASS_H