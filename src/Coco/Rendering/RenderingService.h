#pragma once

#include <Coco/Core/Services/EngineService.h>

#include <Coco/Core/Types/Singleton.h>
#include "Pipeline/RenderPipeline.h"
#include "Graphics/GraphicsPlatform.h"
#include "Graphics/Resources/GraphicsPresenter.h"
#include "Graphics/Resources/RenderContext.h"
#include "Graphics/Resources/Buffer.h"
#include "Providers/ICameraDataProvider.h"
#include "Providers/ISceneDataProvider.h"

namespace Coco::Rendering
{
    class Texture;

    /// @brief A service that allows for rendering operations
    class COCOAPI RenderingService final : public EngineService, public Singleton<RenderingService>
    {
    public:
        /// @brief The priority for the handler that purges cached resources
        static constexpr int ResourcePurgeTickPriority = 9000;

        /// @brief The frequency of cached resource purging (in seconds)
        static constexpr double PurgeFrequency = 1.0;

        static constexpr uint64_t DefaultGraphicsResourceTickLifetime = 10000;

    private:
        ManagedRef<GraphicsPlatform> _graphics;
        Ref<RenderPipeline> _defaultPipeline;
        Ref<Texture> _defaultDiffuseTexture;
        Ref<Texture> _defaultCheckerTexture;

        double _timeSinceLastPurge = 0.0;

    public:
        RenderingService(const GraphicsPlatformCreationParameters& backendCreateParams);
        ~RenderingService() final;

        /// @brief Gets the current graphics platform
        /// @return The current graphics platform
        GraphicsPlatform* GetPlatform() noexcept { return _graphics.Get(); }

        /// @brief Sets the default render pipeline to use
        /// @param pipeline The default render pipeline to use
        void SetDefaultPipeline(Ref<RenderPipeline> pipeline) noexcept { _defaultPipeline = pipeline; }

        /// @brief Gets the default render pipeline
        /// @return The default render pipeline
        Ref<RenderPipeline> GetDefaultPipeline() noexcept { return _defaultPipeline; }

        /// @brief Gets the default diffuse texture
        /// @return The default diffuse texture
        Ref<Texture> GetDefaultDiffuseTexture() noexcept { return _defaultDiffuseTexture; }

        /// @brief Gets the default checker texture
        /// @return The default checker texture
        Ref<Texture> GetDefaultCheckerTexture() noexcept { return _defaultCheckerTexture; }

        /// @brief Renders using the default render pipeline for a graphics presenter
        /// @param presenter The presenter
        /// @param cameraDataProvider The provider for the camera data
        /// @param sceneDataProvider The provider for the scene data
        void Render(
            Ref<GraphicsPresenter> presenter, 
            ICameraDataProvider& cameraDataProvider,
            ISceneDataProvider& sceneDataProvider);

        /// @brief Renders using a render pipeline for a graphics presenter
        /// @param presenter The presenter
        /// @param pipeline The render pipeline
        /// @param cameraDataProvider The provider for the camera data
        /// @param sceneDataProvider The provider for the scene data
        void Render(
            Ref<GraphicsPresenter> presenter, 
            Ref<RenderPipeline> pipeline, 
            ICameraDataProvider& cameraDataProvider, 
            ISceneDataProvider& sceneDataProvider);

    private:
        /// @brief Performs rendering using a render pipeline
        /// @param pipeline The render pipeline
        /// @param context The render context
        void DoRender(RenderPipeline& pipeline, RenderContext& context) noexcept;
                
        /// @brief Creates the default diffuse texture
        void CreateDefaultDiffuseTexture();

        /// @brief Creates the default checker texture
        void CreateDefaultCheckerTexture();

        /// @brief The tick that purges cached resources
        /// @param deltaTime The time since the last tick
        void PurgeTick(double deltaTime);
    };
}

