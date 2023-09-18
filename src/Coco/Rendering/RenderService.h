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

		Ref<Texture> GetDefaultDiffuseTexture() const { return _defaultDiffuseTexture; }
		Ref<Texture> GetDefaultNormalTexture() const { return _defaultNormalTexture; }
		Ref<Texture> GetDefaultCheckerTexture() const { return _defaultCheckerTexture; }

		void Render(Ref<GraphicsPresenter> presenter, RenderPipeline& pipeline, RenderViewProvider& renderViewProvider, std::span<SceneDataProvider*> sceneDataProviders);

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