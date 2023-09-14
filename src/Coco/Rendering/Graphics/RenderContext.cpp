#include "Renderpch.h"
#include "RenderContext.h"
#include "../Pipeline/RenderPass.h"
#include "../Pipeline/CompiledRenderPipeline.h"

namespace Coco::Rendering
{
	ContextRenderOperation::ContextRenderOperation(Rendering::RenderView& renderView, CompiledRenderPipeline& pipeline) :
		RenderView(renderView),
		Pipeline(pipeline),
		CurrentPassIndex(0)
	{}

	const RenderPassBinding& ContextRenderOperation::GetCurrentPass() const
	{
		return Pipeline.RenderPasses.at(CurrentPassIndex);
	}

	void ContextRenderOperation::NextPass()
	{
		CurrentPassIndex = Math::Min(CurrentPassIndex + 1, static_cast<uint32>(Pipeline.RenderPasses.size() - 1));
	}

	RenderContext::RenderContext() :
		_currentState(State::NeedsReset),
		_renderOperation{}
	{
	}

	void RenderContext::Reset()
	{
		_renderOperation.reset();

		if (ResetImpl())
		{
			_currentState = State::ReadyForRender;
		}
	}

	bool RenderContext::Begin(RenderView& renderView, CompiledRenderPipeline& pipeline)
	{
		_renderOperation.emplace(ContextRenderOperation(renderView, pipeline));

		bool began = BeginImpl();

		if (began)
			_currentState = State::InRender;
		else
			_renderOperation.reset();

		return began;
	}

	bool RenderContext::BeginNextPass()
	{
		_renderOperation->NextPass();

		return BeginNextPassImpl();
	}

	void RenderContext::End()
	{
		EndImpl();
		_currentState = State::EndedRender;
	}
}