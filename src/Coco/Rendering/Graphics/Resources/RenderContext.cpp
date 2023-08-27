#include "RenderContext.h"

#include "../../Pipeline/IRenderPass.h"
#include "../../Pipeline/RenderPipeline.h"
#include <Coco/Core/Logging/Logger.h>

namespace Coco::Rendering
{
	RenderContext::RenderContext(const ResourceID& id, const string& name) : RenderingResource(id, name)
	{}

	bool RenderContext::Begin(Ref<Rendering::RenderView> renderView, Ref<RenderPipeline> pipeline)
	{
		if (!IsAvaliableForRendering())
		{
			WaitForRenderingCompleted();
		}

		Reset();

		_currentRenderPipeline = pipeline;
		_currentRenderView = renderView;

		return BeginImpl();
	}

	bool RenderContext::BeginPass(Ref<IRenderPass> renderPass)
	{
		_currentRenderPassIndex++;
		_currentRenderPass = renderPass;

		return BeginPassImpl();
	}

	void RenderContext::End()
	{
		EndImpl();

		_currentShader = ShaderRenderData();
		_currentMaterial = MaterialRenderData();
		_currentRenderView = Ref<Rendering::RenderView>::Empty;
		_currentRenderPipeline = Ref<RenderPipeline>::Empty;
		_currentRenderPass = Ref<IRenderPass>::Empty;
		_currentRenderPassIndex = -1;
	}

	void RenderContext::Reset()
	{
		_currentShader = ShaderRenderData();
		_currentMaterial = MaterialRenderData();
		_currentRenderPass = Ref<IRenderPass>::Empty;
		_currentRenderPassIndex = -1;
		_currentDrawCallCount = 0;
		_currentTrianglesDrawn = 0;

		ResetImpl();
	}

	void RenderContext::RestoreViewport()
	{
		SetViewport(_currentRenderView->ViewportRect);
	}

	void RenderContext::UseShader(const ResourceID& shaderID)
	{
		// No need to change if the same shader is used
		if (_currentShader.ID == shaderID)
			return;

		if (_currentRenderView->Shaders.contains(shaderID))
		{
			const ShaderRenderData& shader = _currentRenderView->Shaders.at(shaderID);

			if (shader.Subshaders.contains(_currentRenderPass->GetSubshaderName()))
			{
				_currentShader = shader;
			}
			else
			{
				LogError(GetRenderingLogger(), FormattedString(
					"Failed to bind shader: Shader {} does not have a subshader for the render pass {}",
					shaderID.ToString(),
					_currentRenderPass->GetSubshaderName()
				));

				_currentShader = ShaderRenderData();
			}
		}

		// Reset shader-based items since we're using a different shader
		ResetShaderUniformData();
		_currentMaterial = MaterialRenderData();

		NewShaderBound();
	}

	void RenderContext::ResetShaderUniformData()
	{
		_currentGlobalUniformData = ShaderUniformData();
		_currentInstanceUniformData = ShaderUniformData();
	}

	void RenderContext::SetShaderVector4(ShaderDescriptorScope scope, const string& name, const Vector4& value)
	{
		switch (scope)
		{
		case ShaderDescriptorScope::Global:
			_currentGlobalUniformData.Vector4s[name] = value;
			break;
		case ShaderDescriptorScope::Instance:
			_currentInstanceUniformData.Vector4s[name] = value;
			break;
		default:
			break;
		}

		UniformUpdated(name);
	}

	void RenderContext::SetShaderColor(ShaderDescriptorScope scope, const string& name, const Color& value)
	{
		switch (scope)
		{
		case ShaderDescriptorScope::Global:
			_currentGlobalUniformData.Colors[name] = value;
			break;
		case ShaderDescriptorScope::Instance:
			_currentInstanceUniformData.Colors[name] = value;
			break;
		default:
			break;
		}

		UniformUpdated(name);
	}

	void RenderContext::SetShaderMatrix4x4(ShaderDescriptorScope scope, const string& name, const Matrix4x4& value)
	{
		switch (scope)
		{
		case ShaderDescriptorScope::Global:
			_currentGlobalUniformData.Matrix4x4s[name] = value;
			break;
		case ShaderDescriptorScope::Instance:
			_currentInstanceUniformData.Matrix4x4s[name] = value;
			break;
		default:
			break;
		}

		UniformUpdated(name);
	}

	void RenderContext::SetShaderTexture(ShaderDescriptorScope scope, const string& name, const ResourceID& textureID)
	{
		switch (scope)
		{
		case ShaderDescriptorScope::Global:
			_currentGlobalUniformData.Textures[name] = textureID;
			break;
		case ShaderDescriptorScope::Instance:
			_currentInstanceUniformData.Textures[name] = textureID;
			break;
		default:
			break;
		}

		TextureSamplerUpdated(name);
	}

	void RenderContext::UseMaterial(const ResourceID& materialID)
	{
		// No need to change if the same material is used
		if (_currentMaterial.ID == materialID)
			return;

		if (_currentRenderView->Materials.contains(materialID))
		{
			const MaterialRenderData& material = _currentRenderView->Materials.at(materialID);

			// Bind the material's shader if it's different than the current one
			if (material.ShaderID != _currentShader.ID)
				UseShader(material.ShaderID);

			_currentMaterial = material;

			_currentInstanceUniformData.Merge(material.UniformData, true);
			_currentInstanceUniformData.ID = material.ID;
			_currentInstanceUniformData.Version = material.Version;
			_currentInstanceUniformData.Preserve = true;
		}
		else
		{
			UseShader(Resource::InvalidID);
		}

		NewMaterialBound();
	}

	void RenderContext::GetLastFrameStats(uint64_t& drawCallCount, uint64_t trianglesDrawn) const noexcept
	{
		drawCallCount = _currentDrawCallCount;
		trianglesDrawn = _currentTrianglesDrawn;
	}
}