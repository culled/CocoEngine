#pragma once

#include <Coco/Core/Resources/Resource.h>

#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Map.h>
#include <Coco/Rendering/ShaderTypes.h>
#include "Resources/VulkanDescriptorSet.h"
#include "VulkanIncludes.h"

namespace Coco::Rendering
{
	class GraphicsDevice;
	class Shader;
}

namespace Coco::Rendering::Vulkan
{
	class GraphicsDeviceVulkan;
	class VulkanDescriptorPool;

	/// @brief A Vulkan shader stage
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

	/// @brief A Vulkan subshader
	struct VulkanSubshader
	{
		/// @brief The stages of this subshader
		List<VulkanShaderStage> ShaderStages;

		/// @brief The descriptor layout of this subshader
		VulkanDescriptorLayout DescriptorLayout = {};
	};

	/// @brief A shader that can be used with Vulkan
	class VulkanShader final : public CachedResource
	{
	private:
		GraphicsDeviceVulkan* _device;
		WeakRef<Shader> _shader;
		Map<string, VulkanSubshader> _subshaders;

	public:
		VulkanShader(GraphicsDeviceVulkan* device, const Ref<Shader>& shader);
		~VulkanShader() final;

		bool IsInvalid() const noexcept final { return _shader.expired(); }
		bool NeedsUpdate() const noexcept final;

		/// @brief Updates the Vulkan shader to reflect the layout of the shader it references
		void Update();

		/// @brief Gets the stages for a subshader
		/// @param subshaderName The name of the subshader
		/// @param stages Will be filled out with a pointer to the subshader
		/// @return True if the subshader was found
		bool TryGetSubshader(const string& subshaderName, const VulkanSubshader*& subshader) const noexcept;

		/// @brief Gets the descriptor layouts for this shader
		/// @return The descriptor layouts for this shader
		List<VulkanDescriptorLayout> GetDescriptorLayouts() const noexcept;

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