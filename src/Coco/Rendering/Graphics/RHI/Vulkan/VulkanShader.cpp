#include "VulkanShader.h"
#include <Coco/Core/Types/Map.h>
#include "GraphicsDeviceVulkan.h"
#include <Coco/Core/IO/File.h>
#include "VulkanUtilities.h"

namespace Coco::Rendering
{
	VulkanShader::VulkanShader(GraphicsDevice* device, const Ref<Shader>& shader) : GraphicsResource(device),
		_device(static_cast<GraphicsDeviceVulkan*>(device))
	{
		List<Subshader> subshaders = shader->GetSubshaders();

		for (const Subshader& subshader : subshaders)
		{
			for (const auto& subshaderStagesKVP : subshader.StageFiles)
			{
				try
				{
					VulkanShaderStage stage = CreateShaderStage(subshaderStagesKVP.first, subshader.PassName, subshaderStagesKVP.second);
					_shaderStages[subshader.PassName].Add(stage);
				}
				catch (Exception& ex)
				{
					string err = FormattedString("Unable to create shader module for \"{}:{}\": {}", shader->GetName(), subshader.PassName, ex.what());
					throw Exception(err.c_str());
				}
			}
		}
	}

	VulkanShader::~VulkanShader()
	{
		for (const auto& shaderKVP : _shaderStages)
		{
			for (const VulkanShaderStage& stage : shaderKVP.second)
			{
				if (stage.ShaderModule != nullptr)
				{
					vkDestroyShaderModule(_device->GetDevice(), stage.ShaderModule, nullptr);
				}
			}
		}

		_shaderStages.clear();
	}

	List<VulkanShaderStage> VulkanShader::GetSubshaderStages(const string& subshaderName) const
	{
		if (!_shaderStages.contains(subshaderName))
			return List<VulkanShaderStage>();

		return _shaderStages.at(subshaderName);
	}

	VulkanShaderStage VulkanShader::CreateShaderStage(ShaderStageType stage, const string& subshaderName, const string& file)
	{
		VulkanShaderStage shaderStage = {};
		shaderStage.EntryPointName = subshaderName;
		shaderStage.StageType = stage;

		shaderStage.ShaderModuleCreateInfo = {};
		shaderStage.ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

		List<uint8_t> byteCode;

		try
		{
			byteCode = File::ReadAllBytes(file);
		}
		catch (Exception& ex)
		{
			string err = FormattedString("Unable to read shader file: {}", ex.what());
			throw Exception(err.c_str());
		}

		shaderStage.ShaderModuleCreateInfo.codeSize = byteCode.Count();
		shaderStage.ShaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(byteCode.Data());

		AssertVkResult(vkCreateShaderModule(_device->GetDevice(), &shaderStage.ShaderModuleCreateInfo, nullptr, &shaderStage.ShaderModule));

		return shaderStage;
	}
}
