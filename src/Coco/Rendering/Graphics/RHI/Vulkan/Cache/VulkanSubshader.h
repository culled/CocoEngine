#pragma once

#include <Coco/Core/Resources/Resource.h>

#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Map.h>
#include <Coco/Rendering/ShaderTypes.h>
#include "../Resources/VulkanDescriptorSet.h"
#include "../VulkanIncludes.h"

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
	class VulkanSubshader
	{
	public:
		/// @brief The shader that this subshader belongs to
		const ResourceID ShaderID;

		/// @brief The name of this subshader
		const string SubshaderName;

	private:
		ResourceVersion _shaderVersion;
		GraphicsDeviceVulkan* _device;
		List<VulkanShaderStage> _shaderStages;
		VulkanDescriptorLayout _descriptorLayout;
		Subshader _subshader;

	public:
		VulkanSubshader(GraphicsDeviceVulkan* device, const ShaderRenderData& shaderData, const string& subshaderName);
		~VulkanSubshader();

		/// @brief Determines if this subshader needs to be updated
		/// @param shaderData The shader data
		/// @return True if this subshader should be updated
		bool NeedsUpdate(const ShaderRenderData& shaderData) const noexcept;

		/// @brief Updates the Vulkan shader to reflect the layout of the subshader it represents
		/// @param shaderData The shader data
		void Update(const ShaderRenderData& shaderData);

		/// @brief Gets the version of this subshader's shader
		/// @return The version of this subshader's shader
		ResourceVersion GetVersion() const noexcept { return _shaderVersion; }

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
}