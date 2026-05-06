//
// Created by cullen on 3/5/26.
//

#include "OpenGLRenderFrame.h"

#include "OpenGLGraphicsPlatform.h"
#include "Resources/OpenGLImage.h"
#include "Coco/Core/Engine.h"
#include "Coco/Rendering/RenderScene.h"
#include "Coco/Rendering/RenderGraph/RenderGraph.h"

namespace Coco
{
    OpenGLRenderFrame::OpenGLRenderFrame(OpenGLGraphicsPlatform* platform) :
        _platform(platform),
        _surfaces(),
        _contexts(),
        _currentContextIndex(0),
        _attachmentImages(),
        _renderSceneStorage(_sceneStoragePageSize),
        _uniformStorage(platform, _sceneStoragePageSize),
        _nextSceneID(0)
    {
        COCO_ENGINE_LOG_VERBOSE("Created OpenGLRenderFrame");
    }

    OpenGLRenderFrame::~OpenGLRenderFrame()
    {
        _renderSceneStorage.Clear();
        _attachmentImages.Clear(true);
        _contexts.Clear(true);
        _surfaces.Clear(true);
        _uniformStorage.Clear();

        COCO_ENGINE_LOG_VERBOSE("Destroyed OpenGLRenderFrame");
    }

    void OpenGLRenderFrame::NewFrame()
    {
        _currentContextIndex = 0;

        for (const auto& image : _attachmentImages)
            _platform->InvalidateResource(image->GetID());

        _attachmentImages.Clear();
        _renderSceneStorage.Clear();
        _uniformStorage.Clear();
        _nextSceneID = 0;
    }

    RenderScene OpenGLRenderFrame::CreateRenderScene()
    {
        return RenderScene(this, _nextSceneID++);
    }

    void OpenGLRenderFrame::Render(RenderGraph&& graph, RenderScene&& scene, Ref<GraphicsSurface> surface)
    {
        Ref<OpenGLGraphicsSurface> glSurface = surface.Downcast<OpenGLGraphicsSurface>();

        EnsureSurfaceRegistered(glSurface);
        if (!glSurface->MakeCurrent())
            return;

        // Add the window surface as the primary color attachment
        const auto& primaryAttachment = graph.GetAttachment(AttachmentType::Color, 0);
        graph.GetStorage().AddExternalTexture(primaryAttachment.ID, glSurface->GetNativeImage());

        CreateAttachmentImages(graph);

        Ref<OpenGLRenderContext> context = GetReadyContext();

        if (!context->Begin(*this, graph, scene))
            return;

        graph.Execute(scene, *context);

        context->End();
    }

    void OpenGLRenderFrame::EndFrame()
    {
        for (auto& surface : _surfaces)
        {
            surface->SwapBuffers();
        }

        _surfaces.Clear();
    }

    void OpenGLRenderFrame::EnsureMeshData(Mesh& mesh)
    {
        _platform->GetMeshStorage()->AddMesh(mesh);
    }

    void OpenGLRenderFrame::EnsureSurfaceRegistered(Ref<OpenGLGraphicsSurface> surface)
    {
        if (_surfaces.Contains(surface))
            return;

        _surfaces.Append(surface);
    }

    Ref<OpenGLRenderContext> OpenGLRenderFrame::GetReadyContext()
    {
        if (_currentContextIndex < _contexts.GetCount())
            return _contexts[_currentContextIndex++];

        Ref<OpenGLRenderContext> ctx = _platform->CreateRenderContext().Downcast<OpenGLRenderContext>();
        _contexts.Append(ctx);
        _currentContextIndex++;
        return ctx;
    }

    void OpenGLRenderFrame::CreateAttachmentImages(RenderGraph& graph)
    {
        CreateAttachmentImages(graph, AttachmentType::Color);
        CreateAttachmentImages(graph, AttachmentType::DepthStencil);
    }

    void OpenGLRenderFrame::CreateAttachmentImages(RenderGraph& graph, AttachmentType type)
    {
        uint8 attachmentCount = graph.GetAttachmentCount(type);
        auto& storage = graph.GetStorage();

        for (uint8 i = 0; i < attachmentCount; i++)
        {
            const auto& textureRes = graph.GetAttachment(type, i);
            if (storage.HasImage(textureRes.ID))
                continue;

            Ref<Image> attachmentImage = _platform->CreateImage(textureRes.Desc);
            storage.AddExternalTexture(textureRes.ID, attachmentImage);
            _attachmentImages.Append(attachmentImage);
        }
    }
} // Coco