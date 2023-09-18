#pragma once
#include "../../../GraphicsDeviceResource.h"
#include "../../../RenderPassShaderTypes.h"
#include "../VulkanDescriptorSetLayout.h"
#include "../VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
	class VulkanGraphicsDevice;

	/// @brief A Vulkan subshader stage
	struct VulkanShaderStage : public ShaderStage
	{
		/// @brief The Vulkan shader module
		VkShaderModule ShaderModule;

		/// @brief The create info for the Vulkan shader module
		VkShaderModuleCreateInfo ShaderModuleCreateInfo;

		VulkanShaderStage(const ShaderStage& stage);
	};

	class VulkanRenderPassShader : public GraphicsDeviceResource<VulkanGraphicsDevice>
	{
	private:
		std::vector<VulkanShaderStage> _stages;
		RenderPassShader _shaderInfo;
		std::unordered_map<UniformScope, VulkanDescriptorSetLayout> _layouts;
		double _lastUsedTime;

	public:
		VulkanRenderPassShader(const RenderPassShader& shaderInfo);
		~VulkanRenderPassShader();

		static GraphicsDeviceResourceID MakeKey(const RenderPassShader& shaderInfo);

		const RenderPassShader& GetInfo() const { return _shaderInfo; }
		std::vector<VulkanDescriptorSetLayout> GetDescriptorSetLayouts() const;
		const VulkanDescriptorSetLayout GetDescriptorSetLayout(UniformScope scope) const;

		uint32 GetUniformDataSize(UniformScope scope) const;
		std::vector<VkPushConstantRange> GetPushConstantRanges() const;

		std::span<const VulkanShaderStage> GetStages() const { return _stages; }

		void Use();
		bool IsStale() const;

	private:
		void CreateStage(const ShaderStage& stage);
		void DestroyShaderStage(const VulkanShaderStage& stage);

		void CreateLayout(UniformScope scope);
		void DestroyLayout(UniformScope scope);
	};
}