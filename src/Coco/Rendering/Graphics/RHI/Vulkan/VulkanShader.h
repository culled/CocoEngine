#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Map.h>
#include <Coco/Rendering/Shader.h>
#include <Coco/Rendering/ShaderTypes.h>
#include <Coco/Rendering/Graphics/GraphicsResource.h>
#include "VulkanIncludes.h"

namespace Coco::Rendering
{
	class GraphicsDevice;
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
	class VulkanShader : public IGraphicsResource
	{
	private:
		GraphicsDeviceVulkan* _device;
		Map<string, List<VulkanShaderStage>> _shaderStages;
		VkDescriptorSetLayout _globalUBODescriptorSetLayout;

	public:
		VulkanShader(GraphicsDevice* device, const Ref<Shader>& shader);
		~VulkanShader();

		/// <summary>
		/// Gets the stages for a subshader
		/// </summary>
		/// <param name="subshaderName">The name of the subshader</param>
		/// <returns>The list of subshader stages</returns>
		List<VulkanShaderStage> GetSubshaderStages(const string& subshaderName) const;

		/// <summary>
		/// Gets the descriptor set layouts for this shader
		/// </summary>
		/// <returns>This shader's descriptor set layouts</returns>
		List<VkDescriptorSetLayout> GetDescriptorSetLayouts() const { return List<VkDescriptorSetLayout>({ _globalUBODescriptorSetLayout }); }

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