#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Services/EngineService.h>

#include "Graphics/GraphicsPlatform.h"
#include "Graphics/GraphicsPresenter.h"
#include "Pipeline/RenderPipeline.h"
#include "Graphics/Buffer.h"
#include "Graphics/GraphicsResource.h"
#include <Coco/Core/Scene/Components/CameraComponent.h>
#include "Texture.h"

namespace Coco::Rendering
{
    /// <summary>
    /// A service that allows for rendering operations
    /// </summary>
    class COCOAPI RenderingService : public EngineService
    {
    private:
        Managed<GraphicsPlatform> _graphics;
        Ref<RenderPipeline> _defaultPipeline;
        Ref<Texture> _defaultTexture;

    public:
        RenderingService(const GraphicsPlatformCreationParameters& backendCreateParams);
        virtual ~RenderingService();

        virtual Logging::Logger* GetLogger() const noexcept override;
        virtual void Start() override;

        /// <summary>
        /// Creates a graphics presenter
        /// </summary>
        /// <returns>A graphics presenter</returns>
        Managed<GraphicsPresenter> CreatePresenter() const { return _graphics->CreatePresenter(); }

        /// <summary>
        /// Gets the current graphics platform
        /// </summary>
        /// <returns>The current graphics platform</returns>
        GraphicsPlatform* GetPlatform() const { return _graphics.get(); }

        /// <summary>
        /// Sets the default render pipeline to use
        /// </summary>
        /// <param name="pipeline">The default render pipeline to use</param>
        void SetDefaultPipeline(Ref<RenderPipeline> pipeline);

        /// <summary>
        /// Gets the default render pipeline
        /// </summary>
        /// <returns>The default render pipeline</returns>
        Ref<RenderPipeline> GetDefaultPipeline() const { return _defaultPipeline; }

        /// <summary>
        /// Renders using the default render pipeline for a graphics presenter
        /// </summary>
        /// <param name="presenter">The presenter</param>
        void Render(GraphicsPresenter* presenter);

        /// <summary>
        /// Renders using a render pipeline for a graphics presenter
        /// </summary>
        /// <param name="presenter">The presenter</param>
        /// <param name="pipeline">The render pipeline</param>
        void Render(GraphicsPresenter* presenter, Ref<RenderPipeline>& pipeline, const Ref<CameraComponent>& camera);

        /// <summary>
        /// Creates a data buffer that can be used to store data on the GPU
        /// </summary>
        /// <param name="size">The size of the buffer (in bytes)</param>
        /// <param name="usageFlags">The usage flags for the buffer</param>
        /// <param name="bindOnCreate">If true, the buffer's memory will be bound once created</param>
        /// <returns>The created buffer</returns>
        GraphicsResourceRef<Buffer> CreateBuffer(uint64_t size, BufferUsageFlags usageFlags, bool bindOnCreate)
        { 
            return _graphics->CreateBuffer(size, usageFlags, bindOnCreate);
        }

        /// <summary>
        /// Creates a two-dimensional texture
        /// </summary>
        /// <param name="width">The texture width</param>
        /// <param name="height">The texture height</param>
        /// <param name="pixelFormat">The pixel format for the texture</param>
        /// <param name="colorSpace">The color space for the texture</param>
        /// <param name="usageFlags">The usage flags for the texture</param>
        /// <returns>The created texture</returns>
        Ref<Texture> CreateTexture(
            int width, 
            int height, 
            PixelFormat pixelFormat, 
            ColorSpace colorSpace, 
            ImageUsageFlags usageFlags,
            FilterMode filterMode = FilterMode::Linear,
            RepeatMode repeatMode = RepeatMode::Repeat,
            uint anisotropy = 16);

        Ref<Texture> GetDefaultTexture() const { return _defaultTexture; }

    private:
        /// <summary>
        /// Performs rendering using a render pipeline
        /// </summary>
        /// <param name="pipeline">The render pipeline</param>
        /// <param name="context">The render context</param>
        void DoRender(const Ref<RenderPipeline>& pipeline, RenderContext* context);

        void CreateDefaultTexture();
    };
}

