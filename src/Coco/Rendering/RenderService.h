#pragma once

#include <Coco/Core/Services/EngineService.h>
#include <Coco/Core/Types/Singleton.h>
#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/MainLoop/TickListener.h>
#include <Coco/Core/Events/Event.h>
#include "Graphics/GraphicsPlatformFactory.h"
#include "Graphics/GraphicsDevice.h"
#include "AttachmentCache.h"

namespace Coco::Rendering
{
	class Presenter;
	class Image;
	class RenderViewDataProvider;
	class SceneDataProvider;
	class Texture;
	class Shader;
	class RenderPipeline;
	struct CompiledRenderPipeline;
	class RenderFrame;

	/// @brief An EngineService that adds rendering functionality
	class RenderService : 
		public EngineService, 
		public Singleton<RenderService>
	{
	public:
		/// @brief The tick priority for the RenderService's early tick
		static constexpr int EarlyTickPriority = -10000;

		/// @brief The tick priority for the RenderService's late tick
		static constexpr int LateTickPriority = 10000;

		/// @brief Gets the name of the error shader
		static const string ErrorShaderName;

		/// @brief Invoked once the maximum frames in flight is changed
		Event<uint32> OnMaxFramesInFlightChanged;

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

		SharedRef<Texture> GetDefaultDiffuseTexture() const { return _defaultDiffuseTexture; }
		SharedRef<Texture> GetDefaultNormalTexture() const { return _defaultNormalTexture; }
		SharedRef<Texture> GetDefaultCheckerTexture() const { return _defaultCheckerTexture; }

		SharedRef<Shader> GetErrorShader() const { return _errorShader; }

		AttachmentCache& GetAttachmentCache() { return *_attachmentCache; }

		/// @brief Queues a render task
		/// @param rendererID The ID of the renderer
		/// @param presenter The presenter that will present the rendered image
		/// @param pipeline The pipeline to use
		/// @param renderViewDataProvider The provider for RenderView data
		/// @param sceneDataProviders The provider for scene data
		/// @return True if the task was queued
		bool Render(
			uint64 rendererID,
			Ref<Presenter> presenter, 
			RenderPipeline& pipeline, 
			RenderViewDataProvider& renderViewDataProvider, 
			std::span<SceneDataProvider*> sceneDataProviders);
		
		/// @brief Queues a render task
		/// @param rendererID The ID of the renderer
		/// @param framebuffers The framebuffers to render to
		/// @param pipeline The pipeline to use
		/// @param renderViewDataProvider The provider for RenderView data
		/// @param sceneDataProviders The provider for scene data
		/// @return True if the task was queued
		bool Render(
			uint64 rendererID,
			std::span<Ref<Image>> framebuffers,
			RenderPipeline& pipeline,
			RenderViewDataProvider& renderViewDataProvider,
			std::span<SceneDataProvider*> sceneDataProviders);

		/// @brief Sets the maximum number of frames in flight for rendering
		/// @param framesInFlight The maximum number of frames in flight for rendering
		void SetMaxFramesInFlight(uint32 framesInFlight);

		/// @brief Gets the maximum number of frames in flight for rendering
		/// @return The maximum number of frames in flight for rendering
		uint32 GetMaxFramesInFlight() const { return _maxFramesInFlight; }

	private:
		UniqueRef<GraphicsPlatform> _platform;
		UniqueRef<GraphicsDevice> _device;
		UniqueRef<AttachmentCache> _attachmentCache;
		SharedRef<Texture> _defaultDiffuseTexture;
		SharedRef<Texture> _defaultNormalTexture;
		SharedRef<Texture> _defaultCheckerTexture;
		SharedRef<Shader> _errorShader;
		//ManagedRef<TickListener> _earlyTickListener;
		ManagedRef<TickListener> _lateTickListener;
		uint32 _maxFramesInFlight;
		std::vector<SharedRef<RenderFrame>> _framesInFlight;

	private:
		void RegisterResourceSerializers();
		void CreateDefaultDiffuseTexture();
		void CreateDefaultNormalTexture();
		void CreateDefaultCheckerTexture();

		/// @brief Attempts to compile a RenderPipeline
		/// @param pipeline The pipeline to compile
		/// @param outCompiledPipeline Will be set to the compiled pipeline if successful
		/// @return True if the pipeline was compiled
		bool TryCompilePipeline(RenderPipeline& pipeline, const CompiledRenderPipeline*& outCompiledPipeline);

		/// @brief Creates the error shader
		void CreateErrorShader();

		/// @brief Handles the early tick
		/// @param tickInfo The current tick info
		//void HandleEarlyTick(const TickInfo& tickInfo);

		/// @brief Handles the late tick
		/// @param tickInfo The current tick info
		void HandleLateTick(const TickInfo& tickInfo);

		SharedRef<RenderFrame> EnsureRenderFrame();
	};
}