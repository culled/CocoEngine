#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Map.h>
#include <Coco/Rendering/Shader.h>
#include <Coco/Rendering/ShaderTypes.h>
#include <Coco/Rendering/Graphics/GraphicsResource.h>
#include "VulkanDescriptorSet.h"
#include "DescriptorPoolVulkan.h"
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
	class VulkanShader final : public IGraphicsResource
	{
	private:
		GraphicsDeviceVulkan* _device;
		Map<string, List<VulkanShaderStage>> _shaderStages;
		Map<string, VulkanDescriptorLayout> _descriptorSetLayouts;
		GraphicsResourceRef<DescriptorPoolVulkan> _descriptorPool;

	public:
		VulkanShader(GraphicsDevice* device, const Shader* shader);
		~VulkanShader() final;

		/// <summary>
		/// Gets the stages for a subshader
		/// </summary>
		/// <param name="subshaderName">The name of the subshader</param>
		/// <param name="stages">A list reference that will be filled with the subshader stages</param>
		/// <returns>True if the subshader was found</returns>
		bool TryGetSubshaderStages(const string& subshaderName, List<VulkanShaderStage>& stages) const noexcept;

		/// <summary>
		/// Gets the descriptor set layout for a subshader
		/// </summary>
		/// <param name="layout">The layout reference that will be set to the subshader's descriptor layout</param>
		/// <returns>True if the subshader was found</returns>
		bool TryGetDescriptorSetLayout(const string& subshaderName, VulkanDescriptorLayout& layout) const noexcept;

		/// <summary>
		/// Gets the descriptor pool
		/// </summary>
		/// <returns>The descriptor pool</returns>
		GraphicsResourceRef<DescriptorPoolVulkan> GetDescriptorPool() const noexcept { return _descriptorPool; }

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