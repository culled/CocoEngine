//
// Created by cullen on 3/4/26.
//

#ifndef COCOENGINE_OPENGLGRAPHICSPLATFORM_H
#define COCOENGINE_OPENGLGRAPHICSPLATFORM_H
#include "../../Graphics/GraphicsPlatform.h"

#include "Coco/Core/Memory/Ptrs.h"
#include "Coco/Core/Types/Map.h"

#include "Coco/Core/Types/Array.h"
#include "Coco/Core/Types/Version.h"
#include "OpenGLMeshStorage.h"

#include <EGL/egl.h>

namespace Coco
{
    class OpenGLGraphicsSurface;
    class OpenGLShaderProgramCompiler;
    class OpenGLRenderFrame;

    class OpenGLGraphicsPlatform : public GraphicsPlatform
    {
    public:
        static constexpr const char* Name = "OpenGL";

    public:
        OpenGLGraphicsPlatform(RenderService* renderService);
        ~OpenGLGraphicsPlatform();

        void EnsureInitialized();

        const char* GetName() const override { return Name; }
        Ref<RenderFrame> GetCurrentRenderFrame() override;
        uint64 GetCurrentFrameNumber() const override { return _currentFrameNumber; }
        void EndCurrentRenderFrame() override;

        Ref<Image> CreateImage(const ImageDescription& imageDescription) override;
        Ref<RenderContext> CreateRenderContext() override;
        Ref<ShaderProgram> CreateShaderProgram(const FilePath& shaderPath) override;
        Ref<Buffer> CreateBuffer(uint64 size, BufferUsageFlags usageFlags) override;
        MeshStorage* GetMeshStorage() override { return _meshStorage.get(); }
        void InvalidateResource(uint64 resourceID) override;

        EGLDisplay GetEGL() const { return _display; }

        Ref<OpenGLGraphicsSurface> CreateSurface(EGLConfig config, EGLSurface surface, EGLContext context, const Sizei& framebufferSize);
        uint8 GetCurrentFrameIndex() const { return _currentRenderFrameIndex; }
        OpenGLShaderProgramCompiler* GetShaderCompiler() { return _shaderProgramCompiler.get(); }

    private:
        EGLDisplay _display;
        Version _platformVersion;
        bool _initialized;
        Array<ManagedRef<OpenGLRenderFrame>> _renderFrames;
        uint8 _currentRenderFrameIndex;
        uint64 _currentFrameNumber;
        Map<uint64, ManagedRef<GraphicsResource>> _resources;
        uint64 _nextResourceID;
        UniquePtr<OpenGLMeshStorage> _meshStorage;
        UniquePtr<OpenGLShaderProgramCompiler> _shaderProgramCompiler;
    };
} // Coco

#endif //COCOENGINE_OPENGLGRAPHICSPLATFORM_H