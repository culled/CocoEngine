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
        static RenderingService* s_instance;

        Managed<GraphicsPlatform> _graphics;
        Ref<RenderPipeline> _defaultPipeline;
        Ref<Texture> _defaultDiffuseTexture;
        Ref<Texture> _defaultCheckerTexture;

    public:
        RenderingService(const GraphicsPlatformCreationParameters& backendCreateParams);
        ~RenderingService() override;

        Logging::Logger* GetLogger() const noexcept override;
        void Start() noexcept override;

        /// <summary>
        /// Gets the active rendering service
        /// </summary>
        /// <returns>The active rendering service</returns>
        static RenderingService* Get() noexcept { return s_instance; }

        /// <summary>
        /// Gets the current graphics platform
        /// </summary>
        /// <returns>The current graphics platform</returns>
        GraphicsPlatform* GetPlatform() const noexcept { return _graphics.get(); }

        /// <summary>
        /// Creates a graphics presenter
        /// </summary>
        /// <returns>A graphics presenter</returns>
        Managed<GraphicsPresenter> CreatePresenter() const { return _graphics->CreatePresenter(); }

        /// <summary>
        /// Sets the default render pipeline to use
        /// </summary>
        /// <param name="pipeline">The default render pipeline to use</param>
        void SetDefaultPipeline(Ref<RenderPipeline> pipeline) noexcept { _defaultPipeline = pipeline; }

        /// <summary>
        /// Gets the default render pipeline
        /// </summary>
        /// <returns>The default render pipeline</returns>
        Ref<RenderPipeline> GetDefaultPipeline() const noexcept { return _defaultPipeline; }

        /// <summary>
        /// Gets the default diffuse texture
        /// </summary>
        /// <returns>The default diffuse texture</returns>
        Ref<Texture> GetDefaultDiffuseTexture() const noexcept { return _defaultDiffuseTexture; }

        /// <summary>
        /// Gets the default checker texture
        /// </summary>
        /// <returns>The default checker texture</returns>
        Ref<Texture> GetDefaultCheckerTexture() const noexcept { return _defaultCheckerTexture; }

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
        void Render(GraphicsPresenter* presenter, Ref<RenderPipeline> pipeline, CameraComponent* camera);

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

    private:
        /// <summary>
        /// Performs rendering using a render pipeline
        /// </summary>
        /// <param name="pipeline">The render pipeline</param>
        /// <param name="context">The render context</param>
        void DoRender(RenderPipeline* pipeline, RenderContext* context) noexcept;

        /// <summary>
        /// Creates the default diffuse texture
        /// </summary>
        void CreateDefaultDiffuseTexture();

        /// <summary>
        /// Creates the default checker texture
        /// </summary>
        void CreateDefaultCheckerTexture();
    };
}

