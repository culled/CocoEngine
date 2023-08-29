#pragma once

#include <Coco/Rendering/Graphics/Resources/GraphicsResource.h>
#include <Coco/Core/Resources/CachedResource.h>

#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Map.h>
#include <Coco/Rendering/ShaderTypes.h>
#include "../../VulkanDescriptorSet.h"
#include "../../VulkanIncludes.h"

namespace Coco::Rendering
{
	struct ShaderRenderData;
}

namespace Coco::Rendering::Vulkan
{
	class GraphicsDeviceVulkan;
	class VulkanDescriptorPool;

	/// @brief A Vulkan subshader stage
	struct VulkanShaderStage : public ShaderStage
	{
		/// @brief The Vulkan shader module
		VkShaderModule ShaderModule = nullptr;

		/// @brief The create info for the Vulkan shader module
		VkShaderModuleCreateInfo ShaderModuleCreateInfo;

		VulkanShaderStage();
	};

	/// @brief A subshader that can be used with Vulkan
	class VulkanSubshader
	{
		friend class VulkanShader;

	private:
		GraphicsDeviceVulkan* _device;
		string _name;
		List<VulkanShaderStage> _shaderStages;
		UnorderedMap<ShaderDescriptorScope, VulkanDescriptorLayout> _descriptorLayouts;
		Subshader _subshader;
		bool _isValid;

	public:
		VulkanSubshader(GraphicsDeviceVulkan* device, const Subshader& subshaderData);
		VulkanSubshader(VulkanSubshader&& other) noexcept;
		~VulkanSubshader();

		VulkanSubshader& operator=(VulkanSubshader&& other) noexcept;

		VulkanSubshader(const VulkanSubshader&) = delete;
		VulkanSubshader& operator=(const VulkanSubshader&) = delete;

		/// @brief Gets the stages for this subshader
		/// @return The stages for this shader
		const List<VulkanShaderStage>& GetStages() const noexcept { return _shaderStages; }

		/// @brief Gets the descriptor layout for this subshader
		/// @param scope The scope of the layout
		/// @return The descriptor layout for this subshader
		const VulkanDescriptorLayout& GetDescriptorLayout(ShaderDescriptorScope scope) const noexcept { return _descriptorLayouts.at(scope); }

		/// @brief Gets the descriptor layouts for this subshader
		/// @return The descriptor layouts for this subshader
		List<VulkanDescriptorLayout> GetDescriptorLayouts() const noexcept;

		/// @brief Gets the size of this subshader's uniform data
		/// @param scope The scope of the uniforms
		/// @return The size of the uniform data
		uint GetUniformDataSize(ShaderDescriptorScope scope) const;

		/// @brief Gets push constant ranges for this subshader
		/// @return Push constant ranges for this subshader
		List<VkPushConstantRange> GetPushConstantRanges() const;

		/// @brief Gets the subshader info for this subshader
		/// @return The subshader info for this subshader
		const Subshader& GetSubshader() const noexcept { return _subshader; }

	private:
		/// @brief Destroys the Vulkan shader objects
		void DestroyShaderObjects() noexcept;

		/// @brief Creates a shader stage from a compiled SPV file
		/// @param stage The shader stage
		/// @return A Vulkan shader stage
		VulkanShaderStage CreateShaderStage(const ShaderStage& stage);

		/// @brief Creates a descriptor layout for the given scope of descriptors
		/// @param scope The scope of the descriptors
		/// @return The created descriptor layout
		VulkanDescriptorLayout CreateDescriptorLayout(ShaderDescriptorScope scope);
	};

	/// @brief A cached Vulkan shader
	class VulkanShader final : public GraphicsResource<GraphicsDeviceVulkan, RenderingResource>, public CachedResource
	{
	private:
		UnorderedMap<string, VulkanSubshader> _subshaders;

	public:
		VulkanShader(const ResourceID& id, const string& name, const ShaderRenderData& shaderData);

		DefineResourceType(VulkanShader)

		bool IsValid() const final { return true; }
		bool NeedsUpdate() const final { return false; }

		/// @brief Determines if this shader needs to be updated
		/// @param shaderData The shader data
		/// @return True if this shader should be updated
		bool NeedsUpdate(const ShaderRenderData& shaderData) const noexcept;

		/// @brief Updates the Vulkan shader to reflect the layout of the shader it represents
		/// @param shaderData The shader data
		void Update(const ShaderRenderData& shaderData);

		/// @brief Gets a subshader of this shader with the given name
		/// @param name The name of the subshader
		/// @param subshader A pointer that will reference the subshader if it was found
		/// @return True if the subshader was found
		bool TryGetSubshader(const string& name, const VulkanSubshader*& subshader) const;

		/// @brief Gets the descriptor layouts of this shader
		/// @return This shader's descriptor layouts
		List<VulkanDescriptorLayout> GetDescriptorLayouts() const;
	};
}