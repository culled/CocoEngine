#pragma once

#include <Coco/Core/Defines.h>
#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/Size.h>

namespace Coco::Rendering
{
	class Presenter;
	class Image;
	struct CompiledRenderPipeline;
	class RenderView;
	class RenderViewDataProvider;
	class SceneDataProvider;
	class Mesh;
	class Material;

	/// @brief Holds data and a series of render tasks for a single frame
	class RenderFrame
	{
	public:
		virtual ~RenderFrame() = default;

		/// @brief Blocks until all this frame's render tasks have completed
		virtual void WaitForRenderToComplete() = 0;
		virtual bool IsRenderComplete() const = 0;

		/// @brief Queues a render task
		/// @param rendererID The ID of the renderer
		/// @param presenter The presenter that will present the rendered image
		/// @param pipeline The pipeline to use
		/// @param renderViewDataProvider The provider for RenderView data
		/// @param sceneDataProviders The provider for scene data
		/// @return True if the task was queued
		virtual bool Render(
			uint64 rendererID,
			Ref<Presenter> presenter,
			const CompiledRenderPipeline& pipeline,
			RenderViewDataProvider& renderViewDataProvider,
			std::span<SceneDataProvider*> sceneDataProviders) = 0;

		/// @brief Queues a render task
		/// @param rendererID The ID of the renderer
		/// @param framebuffers The framebuffers to render to
		/// @param framebufferSize The size of the framebuffers
		/// @param pipeline The pipeline to use
		/// @param renderViewDataProvider The provider for RenderView data
		/// @param sceneDataProviders The provider for scene data
		/// @return True if the task was queued
		virtual bool Render(
			uint64 rendererID,
			std::span<Ref<Image>> framebuffers,
			const SizeInt& framebufferSize,
			const CompiledRenderPipeline& pipeline,
			RenderViewDataProvider& renderViewDataProvider,
			std::span<SceneDataProvider*> sceneDataProviders) = 0;

		virtual void UploadMesh(const SharedRef<Mesh>& mesh) = 0;

	protected:
		SharedRef<RenderView> CreateRenderView(
			uint64 rendererID,
			std::span<Ref<Image>> framebuffers,
			const SizeInt& framebufferSize,
			const CompiledRenderPipeline& compiledPipeline,
			RenderViewDataProvider& renderViewDataProvider,
			std::span<SceneDataProvider*> sceneDataProviders);
	};
}