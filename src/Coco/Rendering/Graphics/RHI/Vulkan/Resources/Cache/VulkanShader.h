#pragma once

#include <Coco/Core/Resources/Resource.h>
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
	struct VulkanShaderStage
	{
		/// @brief The name of the entry point
		string EntryPointName;

		/// @brief The type of this stage
		ShaderStageType StageType;

		/// @brief The Vulkan shader module
		VkShaderModule ShaderModule = nullptr;

		/// @brief The create info for the Vulkan shader module
		VkShaderModuleCreateInfo ShaderModuleCreateInfo;
	};

	/// @brief A subshader that can be used with Vulkan
	struct VulkanSubshader
	{
		friend class VulkanShader;

	private:
		string _name;
		GraphicsDeviceVulkan* _device;
		List<VulkanShaderStage> _shaderStages;
		VulkanDescriptorLayout _descriptorLayout;
		Subshader _subshader;
		bool _isValid;

	public:
		VulkanSubshader(GraphicsDeviceVulkan* device, const Subshader& subshaderData);

		VulkanSubshader(const VulkanSubshader&) = delete;
		VulkanSubshader(VulkanSubshader&& other) noexcept;

		VulkanSubshader& operator=(const VulkanSubshader&) = delete;
		VulkanSubshader& operator=(VulkanSubshader&& other) noexcept;

		~VulkanSubshader();

		/// @brief Gets the stages for this subshader
		/// @return The stages for this shader
		const List<VulkanShaderStage>& GetStages() const noexcept { return _shaderStages; }

		/// @brief Gets the descriptor layout for this subshader
		/// @return The descriptor layout for this subshader
		const VulkanDescriptorLayout& GetDescriptorLayout() const noexcept { return _descriptorLayout; }

		/// @brief Gets the subshader info for this subshader
		/// @return The subshader info for this subshader
		const Subshader& GetSubshader() const noexcept { return _subshader; }

	private:
		/// @brief Destroys the Vulkan shader objects
		void DestroyShaderObjects() noexcept;

		/// @brief Creates a shader stage from a compiled SPV file
		/// @param stage The type of shader stage
		/// @param entrypointName The name of the entrypoint
		/// @param file The path to the shader stage file
		/// @return A Vulkan shader stage
		VulkanShaderStage CreateShaderStage(ShaderStageType stage, const string& entrypointName, const string& file);
	};

	/// @brief A cached collection of subshader resources
	class VulkanShader final : public Resource, public CachedResource
	{
	private:
		GraphicsDeviceVulkan* _device;
		UnorderedMap<string, VulkanSubshader> _subshaders;

	public:
		VulkanShader(ResourceID id, const string& name, uint64_t tickLifetime, GraphicsDeviceVulkan* device, const ShaderRenderData& shaderData);
		VulkanShader(VulkanShader&& other) noexcept;
		VulkanShader& operator=(VulkanShader&& other) noexcept;

		DefineResourceType(VulkanShader)

		bool IsValid() const final { return true; }
		bool NeedsUpdate() const final { return false; }

		/// @brief Determines if this subshader needs to be updated
		/// @param shaderData The shader data
		/// @return True if this subshader should be updated
		bool NeedsUpdate(const ShaderRenderData& shaderData) const noexcept;

		/// @brief Updates the Vulkan shader to reflect the layout of the subshader it represents
		/// @param shaderData The shader data
		void Update(const ShaderRenderData& shaderData);

		VulkanSubshader* GetSubshader(const string& name);

		List<VulkanDescriptorLayout> GetDescriptorLayouts();
	};
}