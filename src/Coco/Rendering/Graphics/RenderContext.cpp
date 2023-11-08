#include "Renderpch.h"
#include "RenderContext.h"
#include "RenderView.h"
#include "../Pipeline/RenderPass.h"
#include "../Pipeline/CompiledRenderPipeline.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	ContextRenderOperation::ContextRenderOperation(const CompiledRenderPipeline& pipeline) :
		Pipeline(pipeline),
		CurrentPassIndex(0)
	{
		CurrentPassName = Pipeline.RenderPasses.at(CurrentPassIndex).Pass->GetName();
	}

	const RenderPassBinding& ContextRenderOperation::GetCurrentPass() const
	{
		return Pipeline.RenderPasses.at(CurrentPassIndex);
	}

	void ContextRenderOperation::NextPass()
	{
		CurrentPassIndex = Math::Min(CurrentPassIndex + 1, static_cast<uint32>(Pipeline.RenderPasses.size() - 1));
		CurrentPassName = Pipeline.RenderPasses.at(CurrentPassIndex).Pass->GetName();
	}

	RenderContext::RenderContext() :
		_currentState(RenderContextState::ReadyForRender),
		_renderOperation{},
		_executionStopwatch(),
		_currentPassStopwatch(),
		_globalUniforms{},
		_globalShaderUniforms{},
		_instanceUniforms{},
		_drawUniforms{},
		_renderView(nullptr)
	{}

	void RenderContext::SetCurrentRenderView(RenderView& renderView)
	{
		_renderView = &renderView;
	}

	void RenderContext::SetTextureSampler(UniformScope scope, ShaderUniformData::UniformKey key, const Ref<Image>& image, const Ref<ImageSampler>& sampler)
	{
		ShaderUniformData::TextureSampler v = ShaderUniformData::ToTextureSampler(image, sampler);

		switch (scope)
		{
		case UniformScope::Global:
			_globalUniforms.Textures[key] = v;
			break;
		case UniformScope::ShaderGlobal:
			_globalShaderUniforms.Textures[key] = v;
			break;
		case UniformScope::Instance:
			_instanceUniforms.Textures[key] = v;
			break;
		case UniformScope::Draw:
			_drawUniforms.Textures[key] = v;
			break;
		default:
			return;
		}

		UniformChanged(scope, key);
	}

	bool RenderContext::Begin(const CompiledRenderPipeline& pipeline)
	{
		Assert(_renderView != nullptr)

		_renderOperation.emplace(ContextRenderOperation(pipeline));

		bool began = BeginImpl();

		if (began)
		{
			_currentState = RenderContextState::InRender;

			_executionStopwatch.Start();
			_currentPassStopwatch.Start();
			
			_stats.FramebufferSize = _renderView->GetViewportRect().GetSize();
		}
		else
			_renderOperation.reset();

		return began;
	}

	bool RenderContext::BeginNextPass()
	{
		Assert(_renderOperation.has_value())

		_stats.PassExecutionTime[_renderOperation->GetCurrentPass().Pass->GetName()] = _currentPassStopwatch.Stop();
		_currentPassStopwatch.Start();

		_renderOperation->NextPass();

		return BeginNextPassImpl();
	}

	void RenderContext::End()
	{
		Assert(_renderOperation.has_value())

		EndImpl();
		
		_stats.PassExecutionTime[_renderOperation->GetCurrentPass().Pass->GetName()] = _currentPassStopwatch.Stop();
		_stats.TotalExecutionTime = _executionStopwatch.Stop();

		_currentState = RenderContextState::EndedRender;
	}

	void RenderContext::Reset()
	{
		ResetImpl();

		_renderView = nullptr;
		_renderOperation.reset();
		_stats.Reset();
		_currentState = RenderContextState::ReadyForRender;

		_globalUniforms.Clear();
		_globalShaderUniforms.Clear();
		_instanceUniforms.Clear();
		_drawUniforms.Clear();
	}
}