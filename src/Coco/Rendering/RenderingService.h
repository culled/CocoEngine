#pragma once

#include <Coco/Core/Services/EngineService.h>

#include "Components/CameraComponent.h"
#include "Pipeline/RenderPipeline.h"
#include "Graphics/GraphicsPlatform.h"
#include "Graphics/Resources/GraphicsPresenter.h"
#include "Graphics/Resources/RenderContext.h"
#include "Graphics/Resources/Buffer.h"

namespace Coco::Rendering
{
    class Texture;

    /// <summary>
    /// A service that allows for rendering operations
    /// </summary>
    class COCOAPI RenderingService final : public EngineService
    {
    public:
        static constexpr int TickPriority = 9000;
        static constexpr double PurgeFrequency = 1.0;

    private:
        static RenderingService* s_instance;

        Managed<GraphicsPlatform> _graphics;
        Ref<RenderPipeline> _defaultPipeline;
        Ref<Texture> _defaultDiffuseTexture;
        Ref<Texture> _defaultCheckerTexture;

        double _timeSinceLastPurge = 0.0;

    public:
        RenderingService(Coco::Engine* engine, const GraphicsPlatformCreationParameters& backendCreateParams);
        ~RenderingService() final;

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
        WeakManagedRef<GraphicsPresenter> CreatePresenter() const { return _graphics->CreatePresenter(); }

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

        void Tick(double deltaTime);
    };
}

