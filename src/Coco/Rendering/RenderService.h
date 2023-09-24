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
#include "RenderStats.h"
#include <Coco/Core/MainLoop/TickListener.h>

namespace Coco::Rendering
{
	/// @brief A render task for a RenderService
	struct RenderServiceRenderTask
	{
		Ref<RenderContext> Context;
		Ref<GraphicsPresenter> Presenter;

		RenderServiceRenderTask(Ref<RenderContext> context, Ref<GraphicsPresenter> presenter);
	};

	/// @brief An EngineService that adds rendering functionality
	class RenderService : public EngineService, public Singleton<RenderService>
	{
	public:
		/// @brief The tick priority for the RenderService's late tick
		static constexpr int sLateTickPriority = 10000;

	private:
		UniqueRef<GraphicsPlatform> _platform;
		UniqueRef<GraphicsDevice> _device;
		Ref<Texture> _defaultDiffuseTexture;
		Ref<Texture> _defaultNormalTexture;
		Ref<Texture> _defaultCheckerTexture;
		RenderStats _stats;
		UniqueRef<TickListener> _lateTickListener;
		std::vector<RenderServiceRenderTask> _currentRenderTasks;

	public:
		RenderService(const GraphicsPlatformFactory& platformFactory);
		~RenderService();

		/// @brief Gets the graphics platform
		/// @return The graphics platform
		GraphicsPlatform& GetPlatform() { return *_platform; }

		/// @brief Gets the graphics platform
		/// @return The graphics platform
		const GraphicsPlatform& GetPlatform() const { return *_platform; }

		/// @brief Gets the graphics device
		/// @return The graphics device
		GraphicsDevice& GetDevice() { return *_device; }

		/// @brief Gets the graphics device
		/// @return The graphics device
		const GraphicsDevice& GetDevice() const { return *_device; }

		/// @brief Gets the default diffuse texture (white)
		/// @return The default diffuse texture
		Ref<Texture> GetDefaultDiffuseTexture() const { return _defaultDiffuseTexture; }

		/// @brief Gets the default normal-map texture
		/// @return The default normal-map texture
		Ref<Texture> GetDefaultNormalTexture() const { return _defaultNormalTexture; }

		/// @brief Gets the default checker texture. Useful if a texture should definitely not be missing
		/// @return The default checker texture
		Ref<Texture> GetDefaultCheckerTexture() const { return _defaultCheckerTexture; }

		/// @brief Gets the current tick's render stats
		/// @return The render stats for the current tick
		const RenderStats& GetRenderStats() const { return _stats; }

		/// @brief Performs a render
		/// @param presenter The presenter to render with
		/// @param pipeline The pipeline to render with
		/// @param renderViewProvider The provider for the RenderView
		/// @param sceneDataProviders The providers for the scene data
		/// @param dependsOn If given, this render will occur after the render associated with the given task
		/// @return A task that can be used for render synchronization
		RenderTask Render(
			Ref<GraphicsPresenter> presenter, 
			RenderPipeline& pipeline, 
			RenderViewProvider& renderViewProvider, 
			std::span<SceneDataProvider*> sceneDataProviders,
			std::optional<RenderTask> dependsOn = std::optional<RenderTask>());

	private:
		/// @brief Performs rendering
		/// @param context The context to render with
		/// @param compiledPipeline The compiled pipeline to render with
		/// @param renderView The view to render with
		/// @param waitOn If given, the render will not start until this semaphore is signaled
		void ExecuteRender(RenderContext& context, CompiledRenderPipeline& compiledPipeline, RenderView& renderView, Ref<GraphicsSemaphore> waitOn);

		/// @brief Creates the default diffuse texture
		void CreateDefaultDiffuseTexture();

		/// @brief Creates the default normal texture
		void CreateDefaultNormalTexture();

		/// @brief Creates the default checker texture
		void CreateDefaultCheckerTexture();

		/// @brief Handles the late tick
		/// @param tickInfo The current tick info
		void HandleLateTick(const TickInfo& tickInfo);
	};
}