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

	/// @brief A Vulkan shader for a specific render pass
	class VulkanRenderPassShader : public GraphicsDeviceResource<VulkanGraphicsDevice>
	{
	private:
		uint64 _version;
		std::vector<VulkanShaderStage> _stages;
		RenderPassShader _shaderInfo;
		std::unordered_map<UniformScope, VulkanDescriptorSetLayout> _layouts;
		double _lastUsedTime;

	public:
		VulkanRenderPassShader(const RenderPassShader& shaderInfo);
		~VulkanRenderPassShader();

		/// @brief Creates a key from the given shader info
		/// @param shaderInfo The shader info
		/// @return The key
		static GraphicsDeviceResourceID MakeKey(const RenderPassShader& shaderInfo);

		/// @brief Gets this shader's version
		/// @return The version
		uint64 GetVersion() const { return _version; }

		/// @brief Gets the info that was used to create this shader
		/// @return The shader info
		const RenderPassShader& GetInfo() const { return _shaderInfo; }

		/// @brief Gets all descriptor set layouts for this shader
		/// @return This shader's descriptor set layouts
		std::vector<VulkanDescriptorSetLayout> GetDescriptorSetLayouts() const;

		/// @brief Gets a descriptor set layout for the given scope
		/// @param scope The uniform scope
		/// @return The descriptor set layout
		const VulkanDescriptorSetLayout& GetDescriptorSetLayout(UniformScope scope) const;

		/// @brief Gets the size of the uniform data for a given scope
		/// @param scope The uniform scope
		/// @return The size of the uniform data, in bytes
		uint32 GetUniformDataSize(UniformScope scope) const;

		/// @brief Gets the push constant ranges for the draw uniforms
		/// @return The push constant ranges
		std::vector<VkPushConstantRange> GetPushConstantRanges() const;

		/// @brief Gets the stages of this shader
		/// @return The shader stages
		std::span<const VulkanShaderStage> GetStages() const { return _stages; }

		/// @brief Determines if this shader needs to be updated
		/// @param shaderInfo The shader info
		/// @return True if this shader should be updated
		bool NeedsUpdate(const RenderPassShader& shaderInfo) const;

		/// @brief Updates this shader from the given shader info
		/// @param shaderInfo The shader info
		void Update(const RenderPassShader& shaderInfo);

		/// @brief Marks this shader as used in the current tick
		void Use();

		/// @brief Determines if this shader is stale and should be removed
		/// @return True if this shader should be removed from the cache
		bool IsStale() const;

	private:
		/// @brief Creates all shader objects
		/// @param shaderInfo The shader info
		void CreateShaderObjects(const RenderPassShader& shaderInfo);

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