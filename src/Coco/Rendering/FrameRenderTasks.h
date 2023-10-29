#pragma once

#include "Renderpch.h"
#include "RenderTask.h"
#include "Graphics/GraphicsPresenter.h"
#include "Graphics/RenderContextPool.h"
#include "Pipeline/CompiledRenderPipeline.h"

namespace Coco::Rendering
{
	/// @brief Render stats for a frame
	struct FrameRenderStats
	{
		/// @brief The number of triangles drawn
		uint64 TrianglesDrawn;

		/// @brief The number of vertices drawn
		uint64 VertexCount;

		/// @brief The number of draw calls
		uint64 DrawCalls;

		/// @brief The amount of time executing render pipelines
		TimeSpan TotalExecutionTime;

		/// @brief The amount of time spent waiting for the previous frame's rendering to complete 
		TimeSpan RenderSyncWait;

		/// @brief The amount of time each render pass took
		std::vector<RenderContextRenderStats> ContextStats;

		FrameRenderStats();

		void operator+=(const RenderContextRenderStats& other);
	};

	/// @brief A container for all render operations for a specific frame
	class FrameRenderTasks
	{
	private:
		uint64 _frameNumber;
		FrameRenderStats _stats;

		std::queue<RenderTask> _tasks;

		std::map<uint64, std::pair<Ref<GraphicsPresenter>, Ref<RenderContext>>> _presenters;
		std::map<uint64, uint64> _firstPresenterTasks;
		std::map<uint64, uint64> _lastPresenterTasks;
		std::map<uint64, CompiledRenderPipeline> _pipelines;


	public:
		FrameRenderTasks(uint64 frameNumber);
		~FrameRenderTasks();

		/// @brief Queues a RenderTask
		/// @param rendererID The ID of the renderer
		/// @param view The view that will be rendered
		/// @param compiledPipeline The pipeline that will be used for rendering
		/// @return True if the task was queued successfully
		bool QueueTask(uint64 rendererID, SharedRef<RenderView> view, const CompiledRenderPipeline& compiledPipeline);

		/// @brief Queues a RenderTask with a presenter that will be presented after all rendering is complete
		/// @param presenter The presenter that will be used for rendering. NOTE: this presenter must have been added with a call to AddPresenter()
		/// @param view The view that will be rendered
		/// @param compiledPipeline The pipeline that will be used for rendering
		/// @return True if the task was queued successfully
		bool QueueTask(Ref<GraphicsPresenter> presenter, SharedRef<RenderView> view, const CompiledRenderPipeline& compiledPipeline);

		/// @brief Adds a presenter that will be used for rendering
		/// @param presenter The presenter
		/// @param firstContext The RenderContext that was used to prepare the presenter for rendering
		/// @param syncWaitTime The time spent waiting for the presenter image to become available
		void AddPresenter(Ref<GraphicsPresenter> presenter, Ref<RenderContext> firstContext, const TimeSpan& syncWaitTime);

		/// @brief Determines if the given presenter is already tracked by this task container
		/// @param presenter The presenter
		/// @return True if this task container is already tracking the presenter. If false, the presenter should be added with AddPresenter() before it is queued for a render
		bool HasPresenter(const Ref<GraphicsPresenter>& presenter) const;

		/// @brief Gets the number of queued tasks
		/// @return The number of queued tasks
		uint64 GetTaskCount() const { return _tasks.size(); }

		/// @brief Executes all queued renders
		/// @param contextPool The pool of RenderContexts
		void Execute(RenderContextPool& contextPool);

		/// @brief Gets the post-execute render stats for this frame
		/// @return The post-excute render stats
		const FrameRenderStats& GetStats() const { return _stats; }

	private:
		/// @brief Performs rendering
		/// @param context The context to render with
		/// @param compiledPipeline The compiled pipeline to render with
		/// @param renderView The view to render with
		/// @param waitOn If given, the render will not start until this semaphore is signaled
		/// @return If true, the render was completed
		bool ExecuteRender(
			RenderContext& context,
			const CompiledRenderPipeline& compiledPipeline,
			RenderView& renderView);
	};
}