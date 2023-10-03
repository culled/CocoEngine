#pragma once

#include <Coco/Core/Services/EngineService.h>
#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/MainLoop/TickListener.h>
#include "Graphics/GraphicsPlatformFactory.h"
#include "Graphics/GraphicsPlatform.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/GraphicsPresenter.h"
#include "Graphics/RenderView.h"
#include "Providers/RenderViewProvider.h"
#include "Providers/SceneDataProvider.h"
#include "RenderTask.h"
#include "RenderStats.h"

namespace Coco::Rendering
{
	class Texture;
	class RenderPipeline;
	class DebugRender;
	struct CompiledRenderPipeline;

	/// @brief A render task for a RenderService
	struct RenderServiceRenderTask
	{
		/// @brief The context used for this task
		Ref<RenderContext> Context;

		/// @brief The presenter, if any, linked to this task
		Ref<GraphicsPresenter> Presenter;

		RenderServiceRenderTask(Ref<RenderContext> context);
		RenderServiceRenderTask(Ref<RenderContext> context, Ref<GraphicsPresenter> presenter);
	};

	/// @brief An EngineService that adds rendering functionality
	class RenderService : public EngineService, public Singleton<RenderService>
	{
	public:
		/// @brief The tick priority for the RenderService's early tick
		static constexpr int sEarlyTickPriority = -10000;

		/// @brief The tick priority for the RenderService's late tick
		static constexpr int sLateTickPriority = 10000;

	private:
		UniqueRef<GraphicsPlatform> _platform;
		UniqueRef<GraphicsDevice> _device;
		UniqueRef<DebugRender> _debugRender;
		Ref<Texture> _defaultDiffuseTexture;
		Ref<Texture> _defaultNormalTexture;
		Ref<Texture> _defaultCheckerTexture;
		RenderStats _stats;
		ManagedRef<TickListener> _earlyTickListener;
		ManagedRef<TickListener> _lateTickListener;
		std::unordered_map<uint64, std::vector<RenderServiceRenderTask>> _renderTasks;
		std::vector<RenderContextRenderStats> _individualRenderStats;
		std::vector<Ref<RenderContext>> _renderContextCache;

		// TODO: when adding multithreaded rendering, support a RenderView pool
		RenderView _renderView;

	public:
		RenderService(const GraphicsPlatformFactory& platformFactory, bool includeDebugRendering);
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

		/// @brief Gets the general stats for all renders since the end of the last tick
		/// @return The general render stats
		const RenderStats& GetRenderStats() const { return _stats; }

		/// @brief Gets the render stats for each render since the end of the last tick
		/// @return The stats for each render
		std::span<const RenderContextRenderStats> GetIndividualRenderStats() const { return _individualRenderStats; }

		/// @brief Performs a render to a GraphicsPresenter's backbuffer
		/// @param presenter The presenter to render with
		/// @param pipeline The pipeline to render with
		/// @param renderViewProvider The provider for the RenderView
		/// @param sceneDataProviders The providers for the scene data
		/// @param dependsOn If given, this render will occur after the render associated with the given task
		/// @param outTask If given, will be filled out with information that can be used for render synchronization
		/// @return True if the render was successful
		bool Render(
			Ref<GraphicsPresenter> presenter, 
			RenderPipeline& pipeline, 
			RenderViewProvider& renderViewProvider, 
			std::span<SceneDataProvider*> sceneDataProviders,
			std::optional<RenderTask> dependsOn = std::optional<RenderTask>(),
			RenderTask* outTask = nullptr);

		/// @brief Performs a render using a list of predefined images
		/// @param rendererID The id of the renderer
		/// @param framebuffers The images to render to
		/// @param pipeline The pipeline to render with
		/// @param renderViewProvider The provider for the RenderView
		/// @param sceneDataProviders The providers for the scene data
		/// @param dependsOn If given, this render will occur after the render associated with the given task
		/// @param outTask If given, will be filled out with information that can be used for render synchronization
		/// @return True if the render was successful
		bool Render(
			uint64 rendererID,
			std::span<Ref<Image>> framebuffers,
			RenderPipeline& pipeline,
			RenderViewProvider& renderViewProvider,
			std::span<SceneDataProvider*> sceneDataProviders,
			std::optional<RenderTask> dependsOn = std::optional<RenderTask>(),
			RenderTask* outTask = nullptr);

		/// @brief Performs a render
		/// @param rendererID The ID of the renderer
		/// @param compiledPipeline The pipeline to use
		/// @param framebuffers The images to render to
		/// @param framebufferSize The size of the frambuffer
		/// @param renderContext The render context to use
		/// @param renderViewProvider The provider for the RenderView
		/// @param sceneDataProviders The providers for the scene data
		/// @param dependsOn If given, this render will occur after the render associated with the given task
		/// @param outTask If given, will be filled out with information that can be used for render synchronization
		/// @return True if the render was successful
		bool Render(
			uint64 rendererID,
			CompiledRenderPipeline& compiledPipeline,
			std::span<Ref<Image>> framebuffers,
			const SizeInt& framebufferSize,
			Ref<RenderContext> renderContext,
			RenderViewProvider& renderViewProvider,
			std::span<SceneDataProvider*> sceneDataProviders,
			std::optional<RenderTask> dependsOn = std::optional<RenderTask>(),
			RenderTask* outTask = nullptr);

	private:
		/// @brief Performs rendering
		/// @param context The context to render with
		/// @param compiledPipeline The compiled pipeline to render with
		/// @param renderView The view to render with
		/// @param waitOn If given, the render will not start until this semaphore is signaled
		/// @return If true, the render was completed and should be saved as a task
		bool ExecuteRender(
			RenderContext& context, 
			CompiledRenderPipeline& compiledPipeline, 
			RenderView& renderView, 
			Ref<GraphicsSemaphore> waitOn);

		/// @brief Creates the default diffuse texture
		void CreateDefaultDiffuseTexture();

		/// @brief Creates the default normal texture
		void CreateDefaultNormalTexture();

		/// @brief Creates the default checker texture
		void CreateDefaultCheckerTexture();

		/// @brief Handles the early tick
		/// @param tickInfo The current tick info
		void HandleEarlyTick(const TickInfo& tickInfo);

		/// @brief Handles the late tick
		/// @param tickInfo The current tick info
		void HandleLateTick(const TickInfo& tickInfo);

		/// @brief Gets/creates a RenderContext that can be used for rendering
		/// @return A RenderContext
		Ref<RenderContext> GetReadyRenderContext();
	};
}