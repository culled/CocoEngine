//
// Created by cullen on 3/5/26.
//

#ifndef COCOENGINE_OPENGLRENDERFRAME_H
#define COCOENGINE_OPENGLRENDERFRAME_H
#include "OpenGLUniformStorage.h"
#include "Resources/OpenGLGraphicsSurface.h"
#include "Resources/OpenGLRenderContext.h"
#include "Coco/Core/Types/Array.h"
#include "Coco/Rendering/Graphics/RenderFrame.h"
#include "Coco/Rendering/Graphics/Resources/ImageTypes.h"

namespace Coco
{
    class OpenGLImage;
    class OpenGLGraphicsPlatform;

    class OpenGLRenderFrame : public RenderFrame
    {
    public:
        OpenGLRenderFrame(OpenGLGraphicsPlatform* platform);
        ~OpenGLRenderFrame();

        void NewFrame() override;
        RenderScene CreateRenderScene() override;
        void Render(RenderGraph&& graph, RenderScene&& scene, Ref<GraphicsSurface> surface) override;
        void EndFrame() override;
        void EnsureMeshData(Mesh& mesh) override;
        RenderSceneStorage* GetSceneStorage() override { return &_renderSceneStorage; }
        const RenderSceneStorage* GetSceneStorage() const override { return &_renderSceneStorage; }
        OpenGLUniformStorage* GetUniformStorage() { return &_uniformStorage; }

        OpenGLGraphicsPlatform* GetPlatform() { return _platform; }

    private:
        static constexpr uint64 _sceneStoragePageSize = 1024 * 1024;

        OpenGLGraphicsPlatform* _platform;
        Array<Ref<OpenGLRenderContext>> _contexts;
        uint64 _currentContextIndex;
        Array<Ref<OpenGLGraphicsSurface>> _surfaces;
        Array<Ref<Image>> _attachmentImages;
        RenderSceneStorage _renderSceneStorage;
        OpenGLUniformStorage _uniformStorage;
        uint64 _nextSceneID;

    private:
        void EnsureSurfaceRegistered(Ref<OpenGLGraphicsSurface> surface);
        Ref<OpenGLRenderContext> GetReadyContext();
        void CreateAttachmentImages(RenderGraph& graph);
        void CreateAttachmentImages(RenderGraph& graph, ImageAttachmentType type);
    };
} // Coco

#endif //COCOENGINE_OPENGLRENDERFRAME_H