#pragma once
#include "CachedVulkanResource.h"
#include "../../../../Shader.h"
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

	/// @brief A Vulkan shader
	class VulkanShader : 
		public CachedVulkanResource
	{
	private:
		uint64 _version;
		SharedRef<Shader> _shader;
		std::vector<VulkanShaderStage> _stages;
		std::unordered_map<UniformScope, VulkanDescriptorSetLayout> _layouts;

	public:
		VulkanShader(const SharedRef<Shader>& shader);
		~VulkanShader();

		static GraphicsDeviceResourceID MakeKey(const SharedRef<Shader>& shader);

		/// @brief Gets the base shader that this shader is based on
		/// @return The shader that this shader is based on
		SharedRef<Shader> GetBaseShader() const { return _shader; }

		/// @brief Gets this shader's version
		/// @return The version
		uint64 GetVersion() const { return _version; }

		/// @brief Gets all descriptor set layouts for this shader
		/// @return This shader's descriptor set layouts
		const std::unordered_map<UniformScope, VulkanDescriptorSetLayout>& GetDescriptorSetLayouts() const { return _layouts; }

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
		/// @return True if this shader should be updated
		bool NeedsUpdate() const;

		/// @brief Updates this shader from the given shader data
		void Update();

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

		/// @brief Creates a descriptor set layout
		/// @param layout The uniform layout
		/// @param scope The scope of the layout
		void CreateLayout(const ShaderUniformLayout& layout, UniformScope scope);

		/// @brief Destroys the descriptor set layout for a uniform scope
		/// @param scope The uniform scope
		void DestroyLayout(UniformScope scope);
	};
}