//
// Created by cullen on 3/4/26.
//

#ifndef COCOENGINE_GRAPHICSPLATFORM_H
#define COCOENGINE_GRAPHICSPLATFORM_H

#include "GraphicsPlatformTypes.h"
#include "MeshStorage.h"
#include "RenderFrame.h"
#include "StagingBuffer.h"
#include "Coco/Core/Memory/Refs.h"
#include "Coco/Rendering/ShaderTypes.h"
#include "Resources/ImageSamplerTypes.h"

namespace Coco
{
    class GraphicsResourceCache;
    class ImageSampler;
    class SlangCompiler;
    class Buffer;
    class ShaderProgram;
    class GraphicsResource;
    class RenderContext;
    class Image;
    struct ImageDescription;
    class RenderService;

    class GraphicsPlatform
    {
    public:
        virtual ~GraphicsPlatform() = default;

        virtual const char* GetName() const = 0;
        virtual Ref<RenderFrame> GetCurrentRenderFrame() = 0;
        virtual uint64 GetCurrentFrameNumber() const = 0;
        virtual void NextFrame() = 0;

        virtual Ref<RenderContext> CreateRenderContext() = 0;
        virtual Ref<Image> CreateImage(const ImageDescription& imageDescription) = 0;
        virtual Ref<ImageSampler> CreateImageSampler(const ImageSamplerDescription& samplerDescription) = 0;
        virtual Ref<ShaderProgram> CreateShaderProgram(const FilePath& shaderPath) = 0;
        virtual Ref<Buffer> CreateBuffer(const BufferDescription& bufferDescription) = 0;
        virtual MeshStorage* GetMeshStorage() = 0;
        virtual StagingBuffer* GetStagingBuffer() = 0;
        virtual SlangCompiler* GetShaderProgramCompiler() = 0;
        virtual GraphicsResourceCache* GetResourceCache() = 0;

        virtual void InvalidateResource(uint64 resourceID) = 0;

        RenderService* GetRenderService() { return _renderService;}
        const GraphicsDeviceDescription& GetDeviceDescription() { return _deviceDescription; }

    protected:
        RenderService* _renderService;
        GraphicsDeviceDescription _deviceDescription;

    protected:
        GraphicsPlatform(RenderService* renderService);
    };
} // Coco

#endif //COCOENGINE_GRAPHICSPLATFORM_H