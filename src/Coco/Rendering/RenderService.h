#pragma once

#include <Coco/Core/Services/EngineService.h>
#include <Coco/Core/Core.h>
#include "Graphics/GraphicsPlatformFactory.h"
#include "Graphics/GraphicsPlatform.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/GraphicsPresenter.h"
#include "Graphics/RenderView.h"
#include "Pipeline/RenderPipeline.h"
#include "Providers/RenderViewProvider.h"
#include "Providers/SceneDataProvider.h"
#include "Texture.h"
#include "RenderTask.h"

namespace Coco::Rendering
{
	/// @brief An EngineService that adds rendering functionality
	class RenderService : public EngineService, public Singleton<RenderService>
	{
	private:
		UniqueRef<GraphicsPlatform> _platform;
		UniqueRef<GraphicsDevice> _device;
		ManagedRef<Texture> _defaultDiffuseTexture;
		ManagedRef<Texture> _defaultNormalTexture;
		ManagedRef<Texture> _defaultCheckerTexture;

	public:
		RenderService(const GraphicsPlatformFactory& platformFactory);
		~RenderService();

		/// @brief Gets the graphics platform
		/// @return The graphics platform
		GraphicsPlatform* GetPlatform() { return _platform.get(); }

		/// @brief Gets the graphics platform
		/// @return The graphics platform
		const GraphicsPlatform* GetPlatform() const { return _platform.get(); }

		/// @brief Gets the graphics device
		/// @return The graphics device
		GraphicsDevice* GetDevice() { return _device.get(); }

		/// @brief Gets the graphics device
		/// @return The graphics device
		const GraphicsDevice* GetDevice() const { return _device.get(); }

		/// @brief Gets the default diffuse texture (white)
		/// @return The default diffuse texture
		Ref<Texture> GetDefaultDiffuseTexture() const { return _defaultDiffuseTexture; }

		/// @brief Gets the default normal-map texture
		/// @return The default normal-map texture
		Ref<Texture> GetDefaultNormalTexture() const { return _defaultNormalTexture; }

		/// @brief Gets the default checker texture. Useful if a texture should definitely not be missing
		/// @return The default checker texture
		Ref<Texture> GetDefaultCheckerTexture() const { return _defaultCheckerTexture; }

		/// @brief Performs a render
		/// @param presenter The presenter to render with
		/// @param pipeline The pipeline to render with
		/// @param renderViewProvider The provider for the RenderView
		/// @param sceneDataProviders The providers for the scene data
		/// @param dependsOn If given, this render will occur after the render associated with the given task
		/// @return A task that can be used for render synchronization
		RenderTask Render(
			GraphicsPresenter& presenter, 
			RenderPipeline& pipeline, 
			RenderViewProvider& renderViewProvider, 
			std::span<SceneDataProvider*> sceneDataProviders,
			std::optional<RenderTask> dependsOn = std::optional<RenderTask>());

	private:
		/// @brief Performs rendering
		/// @param context The context to render with
		/// @param pipeline The pipeline to render with
		/// @param renderView The view to render with
		void ExecuteRender(RenderContext& context, RenderPipeline& pipeline, RenderView& renderView);

		/// @brief Creates the default diffuse texture
		void CreateDefaultDiffuseTexture();

		/// @brief Creates the default normal texture
		void CreateDefaultNormalTexture();

		/// @brief Creates the default checker texture
		void CreateDefaultCheckerTexture();
	};
}