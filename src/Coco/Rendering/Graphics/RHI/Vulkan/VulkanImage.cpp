#include "Renderpch.h"
#include "VulkanImage.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanCommandBuffer.h"
#include "VulkanUtils.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	VulkanImage::VulkanImage(const GraphicsDeviceResourceID& id, const ImageDescription& description, VkImage image) :
		GraphicsDeviceResource<VulkanGraphicsDevice>(id),
		_image(image),
		_isManagedExternally(true),
		_description(description)
	{
		CreateNativeImageView();
	}

	VulkanImage::~VulkanImage()
	{
		_device->WaitForIdle();

		if (_image && !_isManagedExternally)
		{
			vkDestroyImage(_device->GetDevice(), _image, _device->GetAllocationCallbacks());
		}

		if (_nativeView)
		{
			vkDestroyImageView(_device->GetDevice(), _nativeView, _device->GetAllocationCallbacks());
			_nativeView = nullptr;
		}

		_image = nullptr;
	}

	void VulkanImage::TransitionLayout(VulkanCommandBuffer& commandBuffer, VkImageLayout to)
	{
		if (_currentLayout == to || to == VK_IMAGE_LAYOUT_UNDEFINED)
			return;

		DeviceQueue* graphicsQueue = _device->GetQueue(DeviceQueue::Type::Graphics);
		if (!graphicsQueue)
			throw std::exception("Device needs a graphics queue to transition image layouts");

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = _currentLayout;
		barrier.newLayout = to;
		barrier.srcQueueFamilyIndex = graphicsQueue->FamilyIndex;
		barrier.dstQueueFamilyIndex = graphicsQueue->FamilyIndex;
		barrier.image = _image;

		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		switch (_currentLayout)
		{
		case VK_IMAGE_LAYOUT_UNDEFINED:
		{
			barrier.srcAccessMask = 0;
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		{
			barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		{
			barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			sourceStage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
		{
			barrier.srcAccessMask = 0;
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			break;
		}
		default:
			CocoError("Transitioning from {} is unsupported", string_VkImageLayout(_currentLayout));
			return;
		}

		switch (to)
		{
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		{
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		{
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
		{
			barrier.dstAccessMask = 0;
			destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		{
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
			destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		{
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
			destinationStage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			break;
		}
		default:
			CocoError("Transitioning to {} is not supported", string_VkImageLayout(to))
			return;
		}

		_currentLayout = to;

		vkCmdPipelineBarrier(
			commandBuffer.GetCmdBuffer(),
			sourceStage,
			destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);
	}

	void VulkanImage::CreateNativeImageView()
	{
		const bool isDepthFormat = IsDepthFormat(_description.PixelFormat);
		const bool isStencilFormat = IsStencilFormat(_description.PixelFormat);
		const bool isColorFormat = !isDepthFormat && !isStencilFormat;

		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.format = ToVkFormat(_description.PixelFormat, _description.ColorSpace);
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		if (isColorFormat)
		{
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}
		else
		{
			createInfo.subresourceRange.aspectMask = 0;

			if (isDepthFormat)
			{
				createInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
			}

			if (isStencilFormat)
			{
				createInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}

		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = static_cast<uint32_t>(_description.MipCount);
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = static_cast<uint32_t>(_description.Layers);
		createInfo.image = _image;

		AssertVkSuccess(vkCreateImageView(_device->GetDevice(), &createInfo, _device->GetAllocationCallbacks(), &_nativeView));
	}
}