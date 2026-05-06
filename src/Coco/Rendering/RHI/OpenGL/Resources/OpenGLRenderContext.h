//
// Created by cullen on 3/5/26.
//

#ifndef COCOENGINE_OPENGLRENDERCONTEXT_H
#define COCOENGINE_OPENGLRENDERCONTEXT_H
#include "OpenGLGraphicsSurface.h"
#include <Coco/Core/Memory/Refs.h>
#include "../../../Graphics/Resources/RenderContext.h"
#include <EGL/egl.h>

#include "Coco/Core/Types/Array.h"
#include "Coco/Core/Types/Optional.h"
#include "Coco/Rendering/ShaderTypes.h"
#include "Coco/Rendering/Graphics/PagedLinearBuffer.h"
#include "Coco/Rendering/RHI/OpenGL/OpenGLShaderBufferInterface.h"

namespace Coco
{
    class ShaderUniformLayout;
    class OpenGLShaderProgram;
    struct OpenGLUniformBuffer;
    class RenderScene;
    class OpenGLImage;
    class RenderGraph;
    class OpenGLRenderFrame;

    struct OpenGLRenderOperation
    {
        OpenGLRenderFrame* Frame;
        RenderGraph* Graph;
        RenderScene* Scene;
        uint8 BoundColorAttachmentCount;
        Ref<OpenGLShaderProgram> BoundShaderProgram;

        OpenGLRenderOperation(OpenGLRenderFrame& frame, RenderGraph& graph, RenderScene& scene);
    };

    class OpenGLRenderContext : public RenderContext
    {
    public:
        OpenGLRenderContext(OpenGLGraphicsPlatform* platform, uint64 id);
        ~OpenGLRenderContext();

        Sizei GetFramebufferSize() const override;
        void BindAttachmentImages(Span<const Ref<Image>> colorAttachments, Ref<Image> depthStencilAttachment) override;
        void SetViewport(const Recti& viewportRect) override;
        void SetScissor(const Recti& scissorRect) override;
        void SetShader(Shader& shader) override;
        ShaderCursor CreateAndBindGlobalBuffer(const char* name) override;
        void BindGlobalBuffer(const char* name) override;
        ShaderCursor CreateAndBindInstanceBuffer(uint64 instanceID, const char* name) override;
        void BindInstanceBuffer(uint64 instanceID, const char* name) override;
        void DrawObject(const RenderObject& obj) override;

        bool Begin(OpenGLRenderFrame& frame, RenderGraph& graph, RenderScene& scene);
        void End();

    private:
        OpenGLGraphicsPlatform* _platform;
        uint32 _fbo;
        Optional<OpenGLRenderOperation> _operation;
    };
} // Coco

#endif //COCOENGINE_OPENGLRENDERCONTEXT_H