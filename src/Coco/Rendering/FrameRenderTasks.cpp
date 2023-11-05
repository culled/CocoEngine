#include "Renderpch.h"
#include "FrameRenderTasks.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	FrameRenderStats::FrameRenderStats() :
		TrianglesDrawn(0),
		VertexCount(0),
		DrawCalls(0),
		TotalExecutionTime(),
		RenderSyncWait(),
		ContextStats()
	{}

	void FrameRenderStats::operator+=(const RenderContextRenderStats& other)
	{
		TrianglesDrawn += other.TrianglesDrawn;
		VertexCount += other.VertexCount;
		DrawCalls += other.DrawCalls;
		TotalExecutionTime += other.TotalExecutionTime;
		ContextStats.push_back(other);
	}

	FrameRenderTasks::FrameRenderTasks(uint64 frameNumber) :
		_frameNumber(frameNumber),
		_tasks(),
		_presenters(),
		_firstPresenterTasks(),
		_lastPresenterTasks(),
		_pipelines(),
		_stats()
	{}

	FrameRenderTasks::~FrameRenderTasks()
	{
		_presenters.clear();
		_firstPresenterTasks.clear();
		_lastPresenterTasks.clear();
		_pipelines.clear();
	}

	bool FrameRenderTasks::QueueTask(uint64 rendererID, SharedRef<RenderView> view, const CompiledRenderPipeline& compiledPipeline)
	{
		_pipelines.try_emplace(compiledPipeline.PipelineID, compiledPipeline);

		uint64 taskID = Math::CombineHashes(_frameNumber, _tasks.size());
		_tasks.emplace(taskID, rendererID, view, compiledPipeline.PipelineID);
		return true;
	}

	bool FrameRenderTasks::QueueTask(Ref<GraphicsPresenter> presenter, SharedRef<RenderView> view, const CompiledRenderPipeline& compiledPipeline)
	{
		Assert(presenter.IsValid())
		Assert(HasPresenter(presenter))

		bool result = QueueTask(presenter->GetID(), view, compiledPipeline);

		if (result)
		{
			uint64 taskID = _tasks.back().ID;

			if (!_firstPresenterTasks.contains(presenter->GetID()))
			{
				_firstPresenterTasks[presenter->GetID()] = taskID;
			}

			_lastPresenterTasks[presenter->GetID()] = taskID;
		}

		return result;
	}

	void FrameRenderTasks::AddPresenter(Ref<GraphicsPresenter> presenter, Ref<RenderContext> firstContext, const TimeSpan& syncWaitTime)
	{
		Assert(presenter.IsValid())
		_presenters.try_emplace(presenter->GetID(), std::make_pair(presenter, firstContext));
		_stats.RenderSyncWait += syncWaitTime;
	}

	bool FrameRenderTasks::HasPresenter(const Ref<GraphicsPresenter>& presenter) const
	{
		Assert(presenter.IsValid())
		return _presenters.contains(presenter->GetID());
	}

	void FrameRenderTasks::Execute(RenderContextPool& contextPool)
	{
		Ref<RenderContext> lastContext;

		while (!_tasks.empty())
		{
			RenderTask task = _tasks.front();
			_tasks.pop();

			Ref<RenderContext> context;
			Ref<GraphicsPresenter> presenter;
			bool shouldPresent = false;

			// If this task is the first to use a presenter, perform all the setup neccessary
			auto firstPresenterIt = std::find_if(_firstPresenterTasks.begin(), _firstPresenterTasks.end(), [task](const auto& kvp) { return kvp.second == task.ID; });
			if (firstPresenterIt != _firstPresenterTasks.end())
			{
				const auto& presenterInfo = _presenters.at(firstPresenterIt->first);
				presenter = presenterInfo.first;
				context = presenterInfo.second;
			}
			else
			{
				context = contextPool.Get(!lastContext.IsValid());
			}

			if (lastContext.IsValid())
				context->AddWaitOnSemaphore(lastContext->GetRenderCompletedSemaphore());

			// If this task is the last to use a presenter, we should present it after rendering
			auto lastPresenterIt = std::find_if(_lastPresenterTasks.begin(), _lastPresenterTasks.end(), [task](const auto& kvp) { return kvp.second == task.ID; });
			if (lastPresenterIt != _lastPresenterTasks.end())
			{
				const auto& presenterInfo = _presenters.at(lastPresenterIt->first);
				presenter = presenterInfo.first;
				shouldPresent = true;
			}

			const CompiledRenderPipeline& pipeline = _pipelines.at(task.PipelineID);
			bool completedRender = ExecuteRender(*context, pipeline, *task.View);

			if (shouldPresent)
			{
				presenter->Present(context->GetRenderCompletedSemaphore());
				lastContext.Invalidate();
			} 
			else
			{
				lastContext = context;

				if (completedRender && _tasks.empty())
				{
					contextPool.MarkOrphan(context);
				}
			}

			_stats += context->GetRenderStats();
		}
	}

	bool FrameRenderTasks::ExecuteRender(RenderContext& context, const CompiledRenderPipeline& compiledPipeline, RenderView& renderView)
	{
		context.SetCurrentRenderView(renderView);

		try
		{
			// Go through each pass and prepare it
			for (auto it = compiledPipeline.RenderPasses.begin(); it != compiledPipeline.RenderPasses.end(); it++)
			{
				it->Pass->Prepare(context, renderView);
			}
		}
		catch (const std::exception& ex)
		{
			CocoError("Error preparing render: {}", ex.what())

			return false;
		}

		try
		{
			// Go through each pass and execute it
			for (auto it = compiledPipeline.RenderPasses.begin(); it != compiledPipeline.RenderPasses.end(); it++)
			{
				if (it == compiledPipeline.RenderPasses.begin())
				{
					if (!context.Begin(compiledPipeline))
						return false;
				}
				else
				{
					if (!context.BeginNextPass())
						break;
				}

				it->Pass->Execute(context, renderView);
			}
		}
		catch (const std::exception& ex)
		{
			CocoError("Error executing render: {}", ex.what())
		}

		context.End();

		return true;
	}
}