#include "Renderpch.h"
#include "RenderContext.h"
#include "RenderView.h"
#include "../Pipeline/RenderPass.h"
#include "../Pipeline/CompiledRenderPipeline.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	ContextRenderOperation::ContextRenderOperation(CompiledRenderPipeline& pipeline) :
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
		_drawUniforms{}
	{}

	void RenderContext::SetCurrentRenderView(RenderView& renderView)
	{
		_renderView = &renderView;
	}

	void RenderContext::SetFloat(UniformScope scope, ShaderUniformData::UniformKey key, float value)
	{
		switch (scope)
		{
		case UniformScope::Global:
			_globalUniforms.Floats[key] = value;
			break;
		case UniformScope::ShaderGlobal:
			_globalShaderUniforms.Floats[key] = value;
			break;
		case UniformScope::Instance:
			_instanceUniforms.Floats[key] = value;
			break;
		case UniformScope::Draw:
			_drawUniforms.Floats[key] = value;
			break;
		default:
			return;
		}

		UniformChanged(scope, key);
	}

	void RenderContext::SetFloat2(UniformScope scope, ShaderUniformData::UniformKey key, const Vector2& value)
	{
		ShaderUniformData::float2 v = ShaderUniformData::ToFloat2(value);

		switch (scope)
		{
		case UniformScope::Global:
			_globalUniforms.Float2s[key] = v;
			break;
		case UniformScope::ShaderGlobal:
			_globalShaderUniforms.Float2s[key] = v;
			break;
		case UniformScope::Instance:
			_instanceUniforms.Float2s[key] = v;
			break;
		case UniformScope::Draw:
			_drawUniforms.Float2s[key] = v;
			break;
		default:
			return;
		}

		UniformChanged(scope, key);
	}

	void RenderContext::SetFloat3(UniformScope scope, ShaderUniformData::UniformKey key, const Vector3& value)
	{
		ShaderUniformData::float3 v = ShaderUniformData::ToFloat3(value);

		switch (scope)
		{
		case UniformScope::Global:
			_globalUniforms.Float3s[key] = v;
			break;
		case UniformScope::ShaderGlobal:
			_globalShaderUniforms.Float3s[key] = v;
			break;
		case UniformScope::Instance:
			_instanceUniforms.Float3s[key] = v;
			break;
		case UniformScope::Draw:
			_drawUniforms.Float3s[key] = v;
			break;
		default:
			return;
		}

		UniformChanged(scope, key);
	}

	void RenderContext::SetFloat4(UniformScope scope, ShaderUniformData::UniformKey key, const Vector4& value)
	{
		ShaderUniformData::float4 v = ShaderUniformData::ToFloat4(value);

		switch (scope)
		{
		case UniformScope::Global:
			_globalUniforms.Float4s[key] = v;
			break;
		case UniformScope::ShaderGlobal:
			_globalShaderUniforms.Float4s[key] = v;
			break;
		case UniformScope::Instance:
			_instanceUniforms.Float4s[key] = v;
			break;
		case UniformScope::Draw:
			_drawUniforms.Float4s[key] = v;
			break;
		default:
			return;
		}

		UniformChanged(scope, key);
	}

	void RenderContext::SetFloat4(UniformScope scope, ShaderUniformData::UniformKey key, const Color& value, bool asLinear)
	{
		Color v = asLinear ? value.AsLinear() : v = value.AsGamma();
		SetFloat4(scope, key, Vector4(v.R, v.G, v.B, v.A));
	}

	void RenderContext::SetMatrix4x4(UniformScope scope, ShaderUniformData::UniformKey key, const Matrix4x4& value)
	{
		ShaderUniformData::Mat4x4 v = ShaderUniformData::ToMat4x4(value);

		switch (scope)
		{
		case UniformScope::Global:
			_globalUniforms.Mat4x4s[key] = v;
			break;
		case UniformScope::ShaderGlobal:
			_globalShaderUniforms.Mat4x4s[key] = v;
			break;
		case UniformScope::Instance:
			_instanceUniforms.Mat4x4s[key] = v;
			break;
		case UniformScope::Draw:
			_drawUniforms.Mat4x4s[key] = v;
			break;
		default:
			return;
		}

		UniformChanged(scope, key);
	}

	void RenderContext::SetInt(UniformScope scope, ShaderUniformData::UniformKey key, int32 value)
	{
		switch (scope)
		{
		case UniformScope::Global:
			_globalUniforms.Ints[key] = value;
			break;
		case UniformScope::ShaderGlobal:
			_globalShaderUniforms.Ints[key] = value;
			break;
		case UniformScope::Instance:
			_instanceUniforms.Ints[key] = value;
			break;
		case UniformScope::Draw:
			_drawUniforms.Ints[key] = value;
			break;
		default:
			return;
		}

		UniformChanged(scope, key);
	}

	void RenderContext::SetInt2(UniformScope scope, ShaderUniformData::UniformKey key, const Vector2Int& value)
	{
		ShaderUniformData::int2 v = ShaderUniformData::ToInt2(value);

		switch (scope)
		{
		case UniformScope::Global:
			_globalUniforms.Int2s[key] = v;
			break;
		case UniformScope::ShaderGlobal:
			_globalShaderUniforms.Int2s[key] = v;
			break;
		case UniformScope::Instance:
			_instanceUniforms.Int2s[key] = v;
			break;
		case UniformScope::Draw:
			_drawUniforms.Int2s[key] = v;
			break;
		default:
			return;
		}

		UniformChanged(scope, key);
	}

	void RenderContext::SetInt3(UniformScope scope, ShaderUniformData::UniformKey key, const Vector3Int& value)
	{
		ShaderUniformData::int3 v = ShaderUniformData::ToInt3(value);

		switch (scope)
		{
		case UniformScope::Global:
			_globalUniforms.Int3s[key] = v;
			break;
		case UniformScope::ShaderGlobal:
			_globalShaderUniforms.Int3s[key] = v;
			break;
		case UniformScope::Instance:
			_instanceUniforms.Int3s[key] = v;
			break;
		case UniformScope::Draw:
			_drawUniforms.Int3s[key] = v;
			break;
		default:
			return;
		}

		UniformChanged(scope, key);
	}

	void RenderContext::SetInt4(UniformScope scope, ShaderUniformData::UniformKey key, const Vector4Int& value)
	{
		ShaderUniformData::int4 v = ShaderUniformData::ToInt4(value);

		switch (scope)
		{
		case UniformScope::Global:
			_globalUniforms.Int4s[key] = v;
			break;
		case UniformScope::ShaderGlobal:
			_globalShaderUniforms.Int4s[key] = v;
			break;
		case UniformScope::Instance:
			_instanceUniforms.Int4s[key] = v;
			break;
		case UniformScope::Draw:
			_drawUniforms.Int4s[key] = v;
			break;
		default:
			return;
		}

		UniformChanged(scope, key);
	}

	void RenderContext::SetBool(UniformScope scope, ShaderUniformData::UniformKey key, bool value)
	{
		switch (scope)
		{
		case UniformScope::Global:
			_globalUniforms.Bools[key] = value;
			break;
		case UniformScope::ShaderGlobal:
			_globalShaderUniforms.Bools[key] = value;
			break;
		case UniformScope::Instance:
			_instanceUniforms.Bools[key] = value;
			break;
		case UniformScope::Draw:
			_drawUniforms.Bools[key] = value;
			break;
		default:
			return;
		}

		UniformChanged(scope, key);
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

	bool RenderContext::Begin(CompiledRenderPipeline& pipeline)
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

		double time = Engine::cGet()->GetPlatform().GetSeconds();
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