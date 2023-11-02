#pragma once

#include <Coco/Core/Services/EngineService.h>
#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/SharedObjectPool.h>
#include <Coco/Core/MainLoop/TickListener.h>
#include <Coco/Core/Events/Event.h>
#include "Graphics/GraphicsPlatformFactory.h"
#include "Graphics/GraphicsPlatform.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/GraphicsPresenter.h"
#include "Graphics/RenderView.h"
#include "Graphics/AttachmentCache.h"
#include "Providers/RenderViewProvider.h"
#include "Providers/SceneDataProvider.h"
#include "RenderTask.h"
#include "FrameRenderTasks.h"
#include "Graphics/RenderContextPool.h"

namespace Coco::Rendering
{
	class Texture;
	class Shader;
	class RenderPipeline;
	class GizmoRender;
	struct CompiledRenderPipeline;

	/// @brief An EngineService that adds rendering functionality
	class RenderService : public EngineService, public Singleton<RenderService>
	{
	public:
		/// @brief The tick priority for the RenderService's early tick
		static constexpr int sEarlyTickPriority = -10000;

		/// @brief The tick priority for the RenderService's late tick
		static constexpr int sLateTickPriority = 10000;

		Event<uint32> OnMaxFramesInFlightChanged;

	private:
		UniqueRef<GraphicsPlatform> _platform;
		UniqueRef<GraphicsDevice> _device;
		UniqueRef<GizmoRender> _gizmoRender;
		UniqueRef<RenderContextPool> _contextPool;
		bool _renderGizmos;
		SharedRef<Texture> _defaultDiffuseTexture;
		SharedRef<Texture> _defaultNormalTexture;
		SharedRef<Texture> _defaultCheckerTexture;
		SharedRef<Shader> _errorShader;
		ManagedRef<TickListener> _earlyTickListener;
		ManagedRef<TickListener> _lateTickListener;
		AttachmentCache _attachmentCache;
		uint32 _maxFramesInFlight;
		FrameRenderTasks _currentFrameTasks;
		FrameRenderStats _lastFrameStats;
		SharedObjectPool<RenderView> _renderViewPool;

	public:
		RenderService(const GraphicsPlatformFactory& platformFactory, bool includeGizmoRendering);
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
		SharedRef<Texture> GetDefaultDiffuseTexture() const { return _defaultDiffuseTexture; }

		/// @brief Gets the default normal-map texture
		/// @return The default normal-map texture
		SharedRef<Texture> GetDefaultNormalTexture() const { return _defaultNormalTexture; }

		/// @brief Gets the default checker texture. Useful if a texture should definitely not be missing
		/// @return The default checker texture
		SharedRef<Texture> GetDefaultCheckerTexture() const { return _defaultCheckerTexture; }
		
		/// @brief Gets the error shader
		/// @return The error shader
		SharedRef<Shader> GetErrorShader() const { return _errorShader; }

		/// @brief Gets the global attachment cache
		/// @return The global attachment cache
		AttachmentCache& GetAttachmentCache() { return _attachmentCache; }

		/// @brief Queues a render for a GraphicsPresenter
		/// @param presenter The presenter that will display the render
		/// @param pipeline The pipeline to use for rendering
		/// @param renderViewProvider The provider for the RenderView data
		/// @param sceneDataProviders The providers for the scene data
		/// @return True if the render was queued successfully
		bool Render(
			Ref<GraphicsPresenter> presenter, 
			RenderPipeline& pipeline, 
			RenderViewProvider& renderViewProvider, 
			std::span<SceneDataProvider*> sceneDataProviders);

		/// @brief Queues a render
		/// @param rendererID The ID of the renderer
		/// @param framebuffers The provided framebuffers
		/// @param pipeline The pipeline to use for rendering
		/// @param renderViewProvider The provider for the RenderView data
		/// @param sceneDataProviders The providers for the scene data
		/// @return True if the render was queued successfully
		bool Render(
			uint64 rendererID,
			std::span<Ref<Image>> framebuffers,
			RenderPipeline& pipeline,
			RenderViewProvider& renderViewProvider,
			std::span<SceneDataProvider*> sceneDataProviders);

		/// @brief Sets if gizmos should be rendered in subsequent renders
		/// @param renderGizmos If true, gizmos will be rendered
		void SetGizmoRendering(bool renderGizmos);

		/// @brief Gets if gizmos are being rendered
		/// @return True if gizmos will be rendered for each render
		bool GetRenderGizmos() const { return _gizmoRender && _renderGizmos; }

		/// @brief Gets the render stats for the last frame
		/// @return The render stats for the last frame
		const FrameRenderStats& GetLastFrameRenderStats() const { return _lastFrameStats; }

		/// @brief Sets the maximum number of frames in flight for rendering
		/// @param framesInFlight The maximum number of frames in flight for rendering
		void SetMaxFramesInFlight(uint32 framesInFlight);

		/// @brief Gets the maximum number of frames in flight for rendering
		/// @return The maximum number of frames in flight for rendering
		uint32 GetMaxFramesInFlight() const { return _maxFramesInFlight; }

	private:
		/// @brief Gets a RenderView
		/// @param rendererID The ID of the renderer
		/// @param framebuffers The provided framebuffers
		/// @param framebufferSize The size of the framebuffers
		/// @param compiledPipeline The compiled pipeline to use
		/// @param renderViewProvider The provider for the RenderView data
		/// @param sceneDataProviders The providers for the scene data
		/// @return A RenderView
		SharedRef<RenderView> GetRenderView(
			uint64 rendererID,
			std::span<Ref<Image>> framebuffers,
			const SizeInt& framebufferSize,
			const CompiledRenderPipeline& compiledPipeline,
			RenderViewProvider& renderViewProvider,
			std::span<SceneDataProvider*> sceneDataProviders);

		/// @brief Creates the default diffuse texture
		void CreateDefaultDiffuseTexture();

		/// @brief Creates the default normal texture
		void CreateDefaultNormalTexture();

		/// @brief Creates the default checker texture
		void CreateDefaultCheckerTexture();

		/// @brief Creates the error shader
		void CreateErrorShader();

		/// @brief Handles the early tick
		/// @param tickInfo The current tick info
		void HandleEarlyTick(const TickInfo& tickInfo);

		/// @brief Handles the late tick
		/// @param tickInfo The current tick info
		void HandleLateTick(const TickInfo& tickInfo);
	};
}