#pragma once

#include <Coco/Rendering/RenderingExceptions.h>

#include <vulkan/vulkan.h>

namespace Coco::Rendering::Vulkan
{
	/// @brief A generic Vulkan rendering exception
	class VulkanRenderingException : public RenderingException
	{
	public:
		VulkanRenderingException(const string& message);
		virtual ~VulkanRenderingException() = default;
	};

	/// @brief An exception when a Vulkan operation fails
	class VulkanOperationException : public VulkanRenderingException
	{
	public:
		VulkanOperationException(VkResult result, const char* messageFormat = "Vulkan operation failed: {}");
		virtual ~VulkanOperationException() = default;
	};


	/// @brief An exception thrown when a descriptor pool can no longer allocate descriptor sets
	class DescriptorSetAllocateException : public VulkanRenderingException
	{
	public:
		DescriptorSetAllocateException(const string& message);
		virtual ~DescriptorSetAllocateException() = default;
	};
}