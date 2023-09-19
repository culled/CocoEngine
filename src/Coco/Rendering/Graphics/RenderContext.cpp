#include "Renderpch.h"
#include "RenderContext.h"
#include "../Pipeline/RenderPass.h"
#include "../Pipeline/CompiledRenderPipeline.h"

namespace Coco::Rendering
{
	ContextRenderOperation::ContextRenderOperation(Rendering::RenderView& renderView, CompiledRenderPipeline& pipeline) :
		RenderView(renderView),
		Pipeline(pipeline),
		CurrentPassIndex(0),
		GlobalUniforms{},
		InstanceUniforms{},
		DrawUniforms{},
		VerticesDrawn(0),
		TrianglesDrawn(0)
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
		_currentState(State::NeedsReset),
		_renderOperation{}
	{}

	void RenderContext::SetFloat(UniformScope scope, ShaderUniformData::UniformKey key, float value)
	{
		Assert(_renderOperation.has_value())

		switch (scope)
		{
		case UniformScope::Global:
			_renderOperation->GlobalUniforms.Floats[key] = value;
			break;
		case UniformScope::Instance:
			_renderOperation->InstanceUniforms.Floats[key] = value;
			break;
		case UniformScope::Draw:
			_renderOperation->DrawUniforms.Floats[key] = value;
			break;
		default:
			return;
		}

		UniformChanged(scope, key);
	}

	void RenderContext::SetFloat2(UniformScope scope, ShaderUniformData::UniformKey key, const Vector2& value)
	{
		Assert(_renderOperation.has_value())

		ShaderUniformData::float2 v = ShaderUniformData::ToFloat2(value);

		switch (scope)
		{
		case UniformScope::Global:
			_renderOperation->GlobalUniforms.Float2s[key] = v;
			break;
		case UniformScope::Instance:
			_renderOperation->InstanceUniforms.Float2s[key] = v;
			break;
		case UniformScope::Draw:
			_renderOperation->DrawUniforms.Float2s[key] = v;
			break;
		default:
			return;
		}

		UniformChanged(scope, key);
	}

	void RenderContext::SetFloat3(UniformScope scope, ShaderUniformData::UniformKey key, const Vector3& value)
	{
		Assert(_renderOperation.has_value())

		ShaderUniformData::float3 v = ShaderUniformData::ToFloat3(value);

		switch (scope)
		{
		case UniformScope::Global:
			_renderOperation->GlobalUniforms.Float3s[key] = v;
			break;
		case UniformScope::Instance:
			_renderOperation->InstanceUniforms.Float3s[key] = v;
			break;
		case UniformScope::Draw:
			_renderOperation->DrawUniforms.Float3s[key] = v;
			break;
		default:
			return;
		}

		UniformChanged(scope, key);
	}

	void RenderContext::SetFloat4(UniformScope scope, ShaderUniformData::UniformKey key, const Vector4& value)
	{
		Assert(_renderOperation.has_value())

		ShaderUniformData::float4 v = ShaderUniformData::ToFloat4(value);

		switch (scope)
		{
		case UniformScope::Global:
			_renderOperation->GlobalUniforms.Float4s[key] = v;
			break;
		case UniformScope::Instance:
			_renderOperation->InstanceUniforms.Float4s[key] = v;
			break;
		case UniformScope::Draw:
			_renderOperation->DrawUniforms.Float4s[key] = v;
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
		Assert(_renderOperation.has_value())

		ShaderUniformData::Mat4x4 v = ShaderUniformData::ToMat4x4(value);

		switch (scope)
		{
		case UniformScope::Global:
			_renderOperation->GlobalUniforms.Mat4x4s[key] = v;
			break;
		case UniformScope::Instance:
			_renderOperation->InstanceUniforms.Mat4x4s[key] = v;
			break;
		case UniformScope::Draw:
			_renderOperation->DrawUniforms.Mat4x4s[key] = v;
			break;
		default:
			return;
		}

		UniformChanged(scope, key);
	}

	void RenderContext::SetInt(UniformScope scope, ShaderUniformData::UniformKey key, int32 value)
	{
		Assert(_renderOperation.has_value())

		switch (scope)
		{
		case UniformScope::Global:
			_renderOperation->GlobalUniforms.Ints[key] = value;
			break;
		case UniformScope::Instance:
			_renderOperation->InstanceUniforms.Ints[key] = value;
			break;
		case UniformScope::Draw:
			_renderOperation->DrawUniforms.Ints[key] = value;
			break;
		default:
			return;
		}

		UniformChanged(scope, key);
	}

	void RenderContext::SetInt2(UniformScope scope, ShaderUniformData::UniformKey key, const Vector2Int& value)
	{
		Assert(_renderOperation.has_value())

		ShaderUniformData::int2 v = ShaderUniformData::ToInt2(value);

		switch (scope)
		{
		case UniformScope::Global:
			_renderOperation->GlobalUniforms.Int2s[key] = v;
			break;
		case UniformScope::Instance:
			_renderOperation->InstanceUniforms.Int2s[key] = v;
			break;
		case UniformScope::Draw:
			_renderOperation->DrawUniforms.Int2s[key] = v;
			break;
		default:
			return;
		}

		UniformChanged(scope, key);
	}

	void RenderContext::SetInt3(UniformScope scope, ShaderUniformData::UniformKey key, const Vector3Int& value)
	{
		Assert(_renderOperation.has_value())

		ShaderUniformData::int3 v = ShaderUniformData::ToInt3(value);

		switch (scope)
		{
		case UniformScope::Global:
			_renderOperation->GlobalUniforms.Int3s[key] = v;
			break;
		case UniformScope::Instance:
			_renderOperation->InstanceUniforms.Int3s[key] = v;
			break;
		case UniformScope::Draw:
			_renderOperation->DrawUniforms.Int3s[key] = v;
			break;
		default:
			return;
		}

		UniformChanged(scope, key);
	}

	void RenderContext::SetInt4(UniformScope scope, ShaderUniformData::UniformKey key, const Vector4Int& value)
	{
		Assert(_renderOperation.has_value())

		ShaderUniformData::int4 v = ShaderUniformData::ToInt4(value);

		switch (scope)
		{
		case UniformScope::Global:
			_renderOperation->GlobalUniforms.Int4s[key] = v;
			break;
		case UniformScope::Instance:
			_renderOperation->InstanceUniforms.Int4s[key] = v;
			break;
		case UniformScope::Draw:
			_renderOperation->DrawUniforms.Int4s[key] = v;
			break;
		default:
			return;
		}

		UniformChanged(scope, key);
	}

	void RenderContext::SetBool(UniformScope scope, ShaderUniformData::UniformKey key, bool value)
	{
		Assert(_renderOperation.has_value())

		switch (scope)
		{
		case UniformScope::Global:
			_renderOperation->GlobalUniforms.Bools[key] = value;
			break;
		case UniformScope::Instance:
			_renderOperation->InstanceUniforms.Bools[key] = value;
			break;
		case UniformScope::Draw:
			_renderOperation->DrawUniforms.Bools[key] = value;
			break;
		default:
			return;
		}

		UniformChanged(scope, key);
	}

	void RenderContext::SetTextureSampler(UniformScope scope, ShaderUniformData::UniformKey key, const Ref<Image>& image, const Ref<ImageSampler>& sampler)
	{
		Assert(_renderOperation.has_value())

		ShaderUniformData::TextureSampler v = ShaderUniformData::ToTextureSampler(image, sampler);

		switch (scope)
		{
		case UniformScope::Global:
			_renderOperation->GlobalUniforms.Textures[key] = v;
			break;
		case UniformScope::Instance:
			_renderOperation->InstanceUniforms.Textures[key] = v;
			break;
		case UniformScope::Draw:
			_renderOperation->DrawUniforms.Textures[key] = v;
			break;
		default:
			return;
		}

		UniformChanged(scope, key);
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
		Assert(_renderOperation.has_value())

		_renderOperation->NextPass();

		return BeginNextPassImpl();
	}

	void RenderContext::End()
	{
		Assert(_renderOperation.has_value())

		EndImpl();
		_currentState = State::EndedRender;
	}
}