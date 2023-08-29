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
        friend class ManagedRef<RenderingService>;

    public:
        static constexpr uint64_t DefaultGraphicsResourceTickLifetime = 1000;

    private:
        ManagedRef<GraphicsPlatform> _graphics;
        Ref<RenderPipeline> _defaultPipeline;
        Ref<Texture> _defaultDiffuseTexture;
        Ref<Texture> _defaultCheckerTexture;
        Ref<Texture> _defaultNormalTexture;
        List<Ref<ISceneDataProvider>> _sceneDataProviders;

    protected:
        RenderingService(const GraphicsPlatformCreationParameters& backendCreateParams);

    public:
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

        /// @brief Gets the default normal texture
        /// @return The default normal texture
        Ref<Texture> GetDefaultNormalTexture() noexcept { return _defaultNormalTexture; }

        /// @brief Adds a scene data provider to the list of default providers
        /// @param provider The provider
        void AddSceneDataProvider(Ref<ISceneDataProvider> provider);

        /// @brief Removes a scene data provider from the list of default providers
        /// @param provider The provider
        void RemoveSceneDataProvider(Ref<ISceneDataProvider> provider);

        /// @brief Renders using the default render pipeline for a graphics presenter
        /// @param presenter The presenter
        /// @param cameraDataProvider The provider for the camera data
        void Render(
            Ref<GraphicsPresenter> presenter, 
            ICameraDataProvider& cameraDataProvider);

        /// @brief Renders using the default render pipeline for a graphics presenter
        /// @param presenter The presenter
        /// @param cameraDataProvider The provider for the camera data
        /// @param sceneDataProviders The providers for the scene data
        void Render(
            Ref<GraphicsPresenter> presenter,
            ICameraDataProvider& cameraDataProvider,
            List<Ref<ISceneDataProvider>> sceneDataProviders);

        /// @brief Renders using a render pipeline for a graphics presenter
        /// @param presenter The presenter
        /// @param pipeline The render pipeline
        /// @param cameraDataProvider The provider for the camera data
        void Render(
            Ref<GraphicsPresenter> presenter, 
            Ref<RenderPipeline> pipeline, 
            ICameraDataProvider& cameraDataProvider);

        /// @brief Renders using a render pipeline for a graphics presenter
        /// @param presenter The presenter
        /// @param pipeline The render pipeline
        /// @param cameraDataProvider The provider for the camera data
        /// @param sceneDataProviders The providers for the scene data
        void Render(
            Ref<GraphicsPresenter> presenter,
            Ref<RenderPipeline> pipeline,
            ICameraDataProvider& cameraDataProvider,
            List<Ref<ISceneDataProvider>> sceneDataProviders);

    private:
        /// @brief Performs rendering using a render pipeline
        /// @param pipeline The render pipeline
        /// @param context The render context
        void DoRender(RenderPipeline& pipeline, RenderContext& context) noexcept;
                
        /// @brief Creates the default diffuse texture
        void CreateDefaultDiffuseTexture();

        /// @brief Creates the default checker texture
        void CreateDefaultCheckerTexture();

        /// @brief Creates the default normal texture
        void CreateDefaultNormalTexture();
    };
}

