//
// Created by cullen on 3/5/26.
//

#ifndef COCOENGINE_RENDERCONTEXT_H
#define COCOENGINE_RENDERCONTEXT_H
#include "Coco/Core/Math/Rect.h"
#include "Coco/Rendering/Graphics/GraphicsResource.h"
#include <Coco/Core/Types/Span.h>
#include <Coco/Core/Memory/Refs.h>

#include "Coco/Rendering/RenderSceneTypes.h"
#include "Coco/Rendering/Graphics/ShaderCursor.h"
#include "Coco/Rendering/RenderGraph/RenderGraphTypes.h"

namespace Coco
{
    class Shader;
    class Image;

    class RenderContext : public GraphicsResource
    {
    public:
        virtual ~RenderContext() = default;

        virtual Sizei GetFramebufferSize() const = 0;
        virtual void BeginPass(uint64 passIndex, Span<const RenderPassAttachmentInfo> passAttachments) = 0;
        virtual void EndPass() = 0;
        virtual void SetViewport(const Recti& viewportRect) = 0;
        virtual void SetScissor(const Recti& scissorRect) = 0;
        virtual void SetShader(Shader& shader, const GraphicsPipelineState& pipelineState) = 0;
        virtual bool CreateAndBindGlobalBuffer(const char* name, ShaderCursor& outCursor) = 0;
        virtual void BindGlobalBuffer(const char* name) = 0;
        virtual bool CreateAndBindInstanceBuffer(uint64 instanceID, const char* name, ShaderCursor& outCursor) = 0;
        virtual void BindInstanceBuffer(uint64 instanceID, const char* name) = 0;
        virtual void SetDrawData(const void* data, uint64 dataSize, Span<const SharedPtr<Texture>> textures) = 0;
        virtual void DrawObject(const RenderObject& obj) = 0;

    protected:
        RenderContext(uint64 id);
    };
} // Coco

#endif //COCOENGINE_RENDERCONTEXT_H