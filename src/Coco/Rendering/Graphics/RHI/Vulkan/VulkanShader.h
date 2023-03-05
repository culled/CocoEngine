#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Rendering/Graphics/Shader.h>
#include <Coco/Rendering/Graphics/ShaderTypes.h>
#include <Coco/Rendering/Graphics/GraphicsResource.h>
#include "VulkanIncludes.h"

namespace Coco::Rendering
{
	class GraphicsDeviceVulkan;

	/// <summary>
	/// A Vulkan shader stage
	/// </summary>
	struct VulkanShaderStage
	{
		ShaderStageType StageType;
		VkShaderModule ShaderModule;
		VkShaderModuleCreateInfo ShaderModuleCreateInfo;
		VkPipelineShaderStageCreateInfo ShaderStageCreateInfo;
	};

	/// <summary>
	/// A shader that can be used with Vulkan
	/// </summary>
	class VulkanShader : GraphicsResource
	{
	private:
		GraphicsDeviceVulkan* _device;
		List<VulkanShaderStage> _shaderStages;
		//VkPipelineLayout _pipelineLayout;

	public:
		VulkanShader(GraphicsDeviceVulkan* device, const Shader* shader);
		~VulkanShader();

	private:
		/// <summary>
		/// Creates a shader stage from a compiled SPV file
		/// </summary>
		/// <param name="stage">The stage</param>
		/// <param name="file">The path to the file</param>
		/// <returns>The shader stage and module</returns>
		VulkanShaderStage CreateShaderStage(ShaderStageType stage, const string& file);
	};
}