#pragma once

#include <Coco/Rendering/RenderingExceptions.h>

#include <vulkan/vulkan.h>

namespace Coco::Rendering::Vulkan
{
	/// <summary>
	/// A generic Vulkan rendering exception
	/// </summary>
	class VulkanRenderingException : public RenderingException
	{
	public:
		VulkanRenderingException(const string& message);
		virtual ~VulkanRenderingException() = default;
	};

	/// <summary>
	/// An exception when a Vulkan operation fails
	/// </summary>
	class VulkanOperationException : public VulkanRenderingException
	{
	public:
		VulkanOperationException(VkResult result, const char* messageFormat = "Vulkan operation failed: {}");
		virtual ~VulkanOperationException() = default;
	};


	/// <summary>
	/// An exception thrown when a descriptor pool can no longer allocate descriptor sets
	/// </summary>
	class DescriptorSetAllocateException : public VulkanRenderingException
	{
	public:
		DescriptorSetAllocateException(const string& message);
		virtual ~DescriptorSetAllocateException() = default;
	};
}