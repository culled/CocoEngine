//
// Created by cullen on 3/5/26.
//

#include "OpenGLRenderContext.h"
#include "../../../RenderGraph/RenderGraph.h"
#include "../OpenGLRenderFrame.h"
#include "../OpenGLGraphicsPlatform.h"
#include "Coco/Core/Engine.h"

#include "OpenGLBuffer.h"
#include "../OpenGLUtils.h"
#include "OpenGLImage.h"
#include "OpenGLShaderProgram.h"
#include "Coco/Rendering/RenderScene.h"
#include "Coco/Rendering/Shader.h"
#include "Coco/Rendering/Texture.h"
#include "Coco/Rendering/Graphics/ShaderUniformValue.h"
#include "Coco/Rendering/RHI/OpenGL/OpenGLMeshStorage.h"
#include "Coco/Rendering/RHI/OpenGL/OpenGLShaderBufferInterface.h"
#include <glad/glad.h>

namespace Coco
{
    OpenGLRenderOperation::OpenGLRenderOperation(OpenGLRenderFrame& frame, RenderGraph& graph, RenderScene& scene) :
        Frame(&frame),
        Graph(&graph),
        Scene(&scene),
        BoundColorAttachmentCount(0)
    {}

    OpenGLRenderContext::OpenGLRenderContext(OpenGLGraphicsPlatform* platform, uint64 id) :
        RenderContext(id),
        _platform(platform),
        _fbo(0)
    {
        glCreateFramebuffers(1, &_fbo);

        COCO_ENGINE_LOG_VERBOSE("Created OpenGLRenderContext");
    }

    OpenGLRenderContext::~OpenGLRenderContext()
    {
        if (_fbo)
            glDeleteFramebuffers(1, &_fbo);

        COCO_ENGINE_LOG_VERBOSE("Destroyed OpenGLRenderContext");
    }

    Sizei OpenGLRenderContext::GetFramebufferSize() const
    {
        COCO_ASSERT(_operation, "Context wasn't rendering");

        return _operation->Graph->GetAttachmentSize();
    }

    void OpenGLRenderContext::BindAttachmentImages(Span<const Ref<Image>> colorAttachments,
        Ref<Image> depthStencilAttachment)
    {
        COCO_ASSERT(_operation, "Context wasn't rendering");

        for (int i = 0; i < colorAttachments.size(); ++i)
        {
            Ref<OpenGLImage> glImage = colorAttachments[i].Downcast<OpenGLImage>();
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, glImage->GetHandle(), 0);
        }

        // Unbind extra attachments
        for (int i = static_cast<int>(colorAttachments.size()); i < _operation->BoundColorAttachmentCount; ++i)
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, 0, 0);

        _operation->BoundColorAttachmentCount = static_cast<uint8>(colorAttachments.size());

        if (depthStencilAttachment)
        {
            Ref<OpenGLImage> glImage = depthStencilAttachment.Downcast<OpenGLImage>();
            ImagePixelFormat format = glImage->GetDescription().PixelFormat;
            GLenum type = OpenGLUtils::PixelFormatToAttachment(format);

            glFramebufferTexture2D(GL_FRAMEBUFFER, type, GL_TEXTURE_2D, glImage->GetHandle(), 0);
        }
        else
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
        }

        GLenum checkResult = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (checkResult != GL_FRAMEBUFFER_COMPLETE)
        {
            COCO_ASSERT(false, "Framebuffer was malformed: %s (%d)", glGetString(checkResult), checkResult);
        }
    }

    void OpenGLRenderContext::SetViewport(const Recti& viewportRect)
    {
        COCO_ASSERT(_operation, "Context wasn't rendering");
        glViewport(viewportRect.Offset.X, viewportRect.Offset.Y, viewportRect.Size.Width, viewportRect.Size.Height);
    }

    void OpenGLRenderContext::SetScissor(const Recti& scissorRect)
    {
        COCO_ASSERT(_operation, "Context wasn't rendering");
        glScissor(scissorRect.Offset.X, scissorRect.Offset.Y, scissorRect.Size.Width, scissorRect.Size.Height);
    }

    void OpenGLRenderContext::SetShader(Shader& shader)
    {
        COCO_ASSERT(_operation, "Context wasn't rendering");
        Ref<OpenGLShaderProgram> shaderProgram = shader.GetProgram().Downcast<OpenGLShaderProgram>();
        _operation->BoundShaderProgram = shaderProgram;

        shaderProgram->Bind();
    }

    ShaderCursor OpenGLRenderContext::CreateAndBindGlobalBuffer(const char* name)
    {
        COCO_ASSERT(_operation, "Context wasn't rendering");
        COCO_ASSERT(_operation->BoundShaderProgram, "No shader has been bound");

        uint64 id = ShaderUniformValue::NameToHash(name);
        OpenGLShaderBufferInterface* interface = _operation->Frame->GetUniformStorage()->Allocate(id, _operation->BoundShaderProgram->GetParamBlockLayout(name));
        interface->Bind();
        return ShaderCursor(interface);
    }

    void OpenGLRenderContext::BindGlobalBuffer(const char* name)
    {
        COCO_ASSERT(_operation, "Context wasn't rendering");

        uint64 id = ShaderUniformValue::NameToHash(name);
        _operation->Frame->GetUniformStorage()->Bind(id);
    }

    ShaderCursor OpenGLRenderContext::CreateAndBindInstanceBuffer(uint64 instanceID, const char* name)
    {
        COCO_ASSERT(_operation, "Context wasn't rendering");
        COCO_ASSERT(_operation->BoundShaderProgram, "No shader has been bound");

        uint64 id = Math::CombineHashes(instanceID, ShaderUniformValue::NameToHash(name));
        OpenGLShaderBufferInterface* interface = _operation->Frame->GetUniformStorage()->Allocate(id, _operation->BoundShaderProgram->GetParamBlockLayout(name));
        interface->Bind();
        return ShaderCursor(interface);
    }

    void OpenGLRenderContext::BindInstanceBuffer(uint64 instanceID, const char* name)
    {
        uint64 id = Math::CombineHashes(instanceID, ShaderUniformValue::NameToHash(name));
        _operation->Frame->GetUniformStorage()->Bind(id);
    }

    void OpenGLRenderContext::DrawObject(const RenderObject& obj)
    {
        COCO_ASSERT(_operation, "Context wasn't rendering");
        COCO_ASSERT(_operation->BoundShaderProgram, "No shader has been bound");

        OpenGLMeshStorage* meshStorage = static_cast<OpenGLMeshStorage*>(_platform->GetMeshStorage());
        const OpenGLMeshBuffer* meshBuffer = meshStorage->GetMeshBuffer(obj.MeshID);
        glBindVertexArray(meshBuffer->VertexArrayHandle);
        uint64 indexOffset = obj.IndexOffset + meshBuffer->IndexDataOffset;

        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(obj.IndexCount), GL_UNSIGNED_INT, reinterpret_cast<const void*>(indexOffset));
    }

    bool OpenGLRenderContext::Begin(OpenGLRenderFrame& frame, RenderGraph& graph, RenderScene& scene)
    {
        _operation.emplace(frame, graph, scene);

        // Bind our custom framebuffer for rendering
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

        Recti viewportRect(Vector2i::Zero, graph.GetAttachmentSize());
        SetViewport(viewportRect);
        SetScissor(viewportRect);

        // TODO

        return true;
    }

    void OpenGLRenderContext::End()
    {
        glFlush();

        // Bind the default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // TODO

        _operation.reset();
    }
} // Coco