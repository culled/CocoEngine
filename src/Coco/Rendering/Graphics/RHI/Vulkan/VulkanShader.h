#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Map.h>
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
		string EntryPointName;
		ShaderStageType StageType;
		VkShaderModule ShaderModule = nullptr;
		VkShaderModuleCreateInfo ShaderModuleCreateInfo;
	};

	/// <summary>
	/// A shader that can be used with Vulkan
	/// </summary>
	class VulkanShader : public GraphicsResource
	{
	private:
		GraphicsDeviceVulkan* _device;
		Map<string, List<VulkanShaderStage>> _shaderStages;

	public:
		VulkanShader(GraphicsDevice* device, const Ref<Shader>& shader);
		~VulkanShader();

		/// <summary>
		/// Gets the stages for a subshader
		/// </summary>
		/// <param name="subshaderName">The name of the subshader</param>
		/// <returns>The list of subshader stages</returns>
		List<VulkanShaderStage> GetSubshaderStages(const string& subshaderName) const;

	private:
		/// <summary>
		/// Creates a shader stage from a compiled SPV file
		/// </summary>
		/// <param name="stage">The stage</param>
		/// <param name="file">The path to the file</param>
		/// <returns>The shader stage and module</returns>
		VulkanShaderStage CreateShaderStage(ShaderStageType stage, const string& subshaderName, const string& file);
	};
}