#include "VulkanShader.h"
#include <Coco/Core/Types/Map.h>
#include "GraphicsDeviceVulkan.h"
#include <Coco/Core/IO/File.h>

namespace Coco::Rendering
{
	VulkanShader::VulkanShader(GraphicsDeviceVulkan* device, const Shader* shader) : GraphicsResource(device),
		_device(device)
	{
		Map<ShaderStageType, string> shaderStages = shader->GetStageSources();

		for (const auto& kvp : shaderStages)
		{
			try
			{
				VulkanShaderStage stage = CreateShaderStage(kvp.first, kvp.second);
			}
			catch (Exception& ex)
			{
				string err = FormattedString("Unable to create shader: {}", ex.what());
				throw Exception(err.c_str());
			}
		}
	}

	VulkanShader::~VulkanShader()
	{
	}

	VulkanShaderStage VulkanShader::CreateShaderStage(ShaderStageType stage, const string& file)
	{
		VulkanShaderStage shaderStage = {};
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

		shaderStage.ShaderStageCreateInfo = {};
		shaderStage.ShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStage.ShaderStageCreateInfo.flags = VkShaderStageFlags(stage);
		shaderStage.ShaderStageCreateInfo.module = shaderStage.ShaderModule;
		shaderStage.ShaderStageCreateInfo.pName = "main"; // TODO: configurable entry point

		return shaderStage;
	}
}
