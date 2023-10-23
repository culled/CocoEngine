#pragma once
#include "CachedVulkanResource.h"
#include "../../../ShaderVariant.h"
#include "../../../RenderViewTypes.h"
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

	/// @brief A Vulkan shader for a specific render pass
	class VulkanShaderVariant : 
		public CachedVulkanResource
	{
	private:
		uint64 _version;
		std::vector<VulkanShaderStage> _stages;
		ShaderVariant _variant;
		std::map<UniformScope, VulkanDescriptorSetLayout> _layouts;

	public:
		VulkanShaderVariant(const ShaderVariantData& variantData);
		~VulkanShaderVariant();

		/// @brief Creates a key from the given shader info
		/// @param variantData The shader variant data
		/// @return The key
		static GraphicsDeviceResourceID MakeKey(const ShaderVariantData& variantData);

		/// @brief Gets this shader's version
		/// @return The version
		uint64 GetVersion() const { return _version; }

		/// @brief Gets the variant that was used to create this shader
		/// @return The shader variant
		const ShaderVariant& GetVariant() const { return _variant; }

		/// @brief Gets all descriptor set layouts for this shader
		/// @return This shader's descriptor set layouts
		std::vector<VulkanDescriptorSetLayout> GetDescriptorSetLayouts() const;

		/// @brief Gets a descriptor set layout for the given scope
		/// @param scope The uniform scope
		/// @return The descriptor set layout
		const VulkanDescriptorSetLayout& GetDescriptorSetLayout(UniformScope scope) const;

		/// @brief Determines if this shader has a descriptor set layout for the given scope
		/// @param scope The uniform scope
		/// @return True if this shader has a layout for the given scope
		bool HasScope(UniformScope scope) const;

		/// @brief Gets the push constant ranges for the draw uniforms
		/// @return The push constant ranges
		std::vector<VkPushConstantRange> GetPushConstantRanges() const;

		/// @brief Gets the stages of this shader
		/// @return The shader stages
		std::span<const VulkanShaderStage> GetStages() const { return _stages; }

		/// @brief Determines if this shader needs to be updated
		/// @param variantData The shader variant
		/// @return True if this shader should be updated
		bool NeedsUpdate(const ShaderVariantData& variantData) const;

		/// @brief Updates this shader from the given shader variant
		/// @param variantData The shader variant
		void Update(const ShaderVariantData& variantData);

	private:
		/// @brief Creates all shader objects
		void CreateShaderObjects();

		/// @brief Destroys all shader objects
		void DestroyShaderObjects();

		/// @brief Creates a shader stage
		/// @param stage The stage
		void CreateStage(const ShaderStage& stage);

		/// @brief Destroys a shader stage
		/// @param stage The stage
		void DestroyShaderStage(const VulkanShaderStage& stage);

		/// @brief Creates a descriptor set layout for a uniform scope
		/// @param scope The uniform scope
		void CreateLayout(UniformScope scope);

		/// @brief Destroys the descriptor set layout for a uniform scope
		/// @param scope The uniform scope
		void DestroyLayout(UniformScope scope);
	};
}