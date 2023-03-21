#pragma once

#include <Coco/Core/Resources/Resource.h>

#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Map.h>
#include <Coco/Rendering/ShaderTypes.h>
#include "VulkanIncludes.h"

namespace Coco::Rendering
{
	class GraphicsDevice;
	class Shader;
}

namespace Coco::Rendering::Vulkan
{
	class GraphicsDeviceVulkan;
	struct VulkanDescriptorLayout;
	class VulkanDescriptorPool;

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
	class VulkanShader final : public CachedResource
	{
	private:
		GraphicsDeviceVulkan* _device;
		WeakRef<Shader> _shader;
		Map<string, List<VulkanShaderStage>> _shaderStages;
		Map<string, VulkanDescriptorLayout> _descriptorSetLayouts;

	public:
		VulkanShader(GraphicsDeviceVulkan* device, Ref<Shader> shader);
		~VulkanShader() final;

		bool IsInvalid() const noexcept final { return _shader.expired(); }
		bool NeedsUpdate() const noexcept final;

		void Update();

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
		/// Gets the descriptor layouts for this shader
		/// </summary>
		/// <returns>The descriptor layouts for this shader</returns>
		List<VulkanDescriptorLayout> GetDescriptorLayouts() const noexcept;

	private:
		void DestroyShaderObjects() noexcept;

		/// <summary>
		/// Creates a shader stage from a compiled SPV file
		/// </summary>
		/// <param name="stage">The stage</param>
		/// <param name="file">The path to the file</param>
		/// <returns>The shader stage and module</returns>
		VulkanShaderStage CreateShaderStage(ShaderStageType stage, const string& subshaderName, const string& file);
	};
}