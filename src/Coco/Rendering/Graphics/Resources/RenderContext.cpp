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
		_globalUO = GlobalUniformObject(renderView.Get());

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

		const ShaderRenderData& shader = _currentRenderView->Shaders.at(shaderID);

		if(shader.Subshaders.contains(_currentRenderPass->GetSubshaderName()))
		{
			_currentShader = shader;
		}
		else
		{
			LogError(GetRenderingLogger(), FormattedString(
				"Failed to bind shader: Shader {} does not have a subshader for the render pass {}", 
				shaderID.AsString(),
				_currentRenderPass->GetSubshaderName()
			));

			_currentShader = ShaderRenderData();
		}

		// Reset shader-based items since we're using a different shader
		ResetShaderUniformData();
		_currentMaterial = MaterialRenderData();

		NewShaderBound();
	}

	void RenderContext::ResetShaderUniformData()
	{
		_currentShaderUniformData = ShaderUniformData();
	}

	void RenderContext::SetShaderVector4(const string& name, const Vector4& value)
	{
		_currentShaderUniformData.Vector4s[name] = value;

		UniformUpdated(name);
	}

	void RenderContext::SetShaderColor(const string& name, const Color& value)
	{
		_currentShaderUniformData.Colors[name] = value;

		UniformUpdated(name);
	}

	void RenderContext::SetShaderTexture(const string& name, const ResourceID& textureID)
	{
		_currentShaderUniformData.Textures[name] = textureID;

		TextureSamplerUpdated(name);
	}

	void RenderContext::UseMaterial(const ResourceID& materialID)
	{
		// No need to change if the same material is used
		if (_currentMaterial.ID == materialID)
			return;

		const MaterialRenderData& material = _currentRenderView->Materials.at(materialID);

		// Bind the material's shader if it's different than the current one
		if (material.ShaderID != _currentShader.ID)
			UseShader(material.ShaderID);

		_currentMaterial = material;

		_currentShaderUniformData.Merge(material.UniformData, true);
		_currentShaderUniformData.ID = material.ID;
		_currentShaderUniformData.Version = material.Version;
		_currentShaderUniformData.Preserve = true;

		NewMaterialBound();
	}

	void RenderContext::GetLastFrameStats(uint64_t& drawCallCount, uint64_t trianglesDrawn) const noexcept
	{
		drawCallCount = _currentDrawCallCount;
		trianglesDrawn = _currentTrianglesDrawn;
	}
}