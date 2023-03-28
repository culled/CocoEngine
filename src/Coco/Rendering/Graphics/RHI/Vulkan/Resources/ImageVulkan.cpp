#include "ImageVulkan.h"

#include <Coco/Core/Logging/Logger.h>
#include "../GraphicsDeviceVulkan.h"
#include "../GraphicsPlatformVulkan.h"
#include "../VulkanUtilities.h"
#include "../VulkanQueue.h"
#include "BufferVulkan.h"
#include "CommandBufferPoolVulkan.h"
#include "CommandBufferVulkan.h"

#include <vulkan/vk_enum_string_helper.h>

namespace Coco::Rendering::Vulkan
{
	ImageVulkan::ImageVulkan(GraphicsDevice* device, ImageDescription description, VkImage image) : 
		Image(description),
		_device(static_cast<GraphicsDeviceVulkan*>(device)), _image(image), _isManagedInternally(true)
	{
		CreateNativeImageView();
	}

	ImageVulkan::ImageVulkan(GraphicsDevice* device, ImageDescription description) : Image(description),
		_device(static_cast<GraphicsDeviceVulkan*>(device)), _isManagedInternally(false)
	{
		CreateImageFromDescription();
		CreateNativeImageView();
	}

	ImageVulkan::~ImageVulkan()
	{
		_device->WaitForIdle();

		if(_nativeView != nullptr)
			vkDestroyImageView(_device->GetDevice(), _nativeView, nullptr);

		if (!_isManagedInternally && _image != nullptr)
		{
			vkFreeMemory(_device->GetDevice(), _imageMemory, nullptr);
			vkDestroyImage(_device->GetDevice(), _image, nullptr);
		}

		_nativeView = nullptr;
		_image = nullptr;
		_imageMemory = nullptr;
	}

	void ImageVulkan::SetPixelData(uint64_t offset, uint64_t size, const void* pixelData)
	{
		CommandBufferPoolVulkan* pool;
		if (!_device->GetGraphicsCommandPool(pool))
			throw VulkanRenderingException("A graphics queue is required to transfer pixel data");

		const uint64_t imageSize = GetSize();

		if (offset + size > imageSize)
			throw IndexOutOfRangeException(FormattedString("The combination of offset and size must fall within the image memory. 0 < {} <= {}",
				offset + size,
				imageSize
			));

		WeakManagedRef<BufferVulkan> staging = _device->CreateResource<BufferVulkan>(
			BufferUsageFlags::HostVisible | BufferUsageFlags::TransferSource | BufferUsageFlags::TransferDestination,
			imageSize,
			true);

		staging->LoadData(offset, size, pixelData);

		WeakManagedRef<CommandBufferVulkan> commandBuffer = pool->Allocate(true);
		commandBuffer->Begin(true, false);

		TransitionLayout(commandBuffer.Get(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		CopyFromBuffer(commandBuffer.Get(), staging.Get());
		TransitionLayout(commandBuffer.Get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		commandBuffer->EndAndSubmit();
		pool->WaitForQueue();
		pool->Free(commandBuffer);

		staging.Invalidate();
	}

	void ImageVulkan::CopyFromBuffer(const CommandBufferVulkan* commandBuffer, BufferVulkan* buffer)
	{
		VkBufferImageCopy region = {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageExtent.width = static_cast<uint32_t>(Description.Width);
		region.imageExtent.height = static_cast<uint32_t>(Description.Height);
		region.imageExtent.depth = static_cast<uint32_t>(Description.Depth);

		vkCmdCopyBufferToImage(commandBuffer->GetCmdBuffer(), buffer->GetBuffer(), _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	}

	void ImageVulkan::TransitionLayout(const CommandBufferVulkan* commandBuffer, VkImageLayout from, VkImageLayout to)
	{
		Ref<VulkanQueue> graphicsQueue;

		if (!_device->GetComputeQueue(graphicsQueue))
			throw VulkanRenderingException("Device needs a graphics queue to transition image layouts");

		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = from;
		barrier.newLayout = to;
		barrier.srcQueueFamilyIndex = graphicsQueue->QueueFamily;
		barrier.dstQueueFamilyIndex = graphicsQueue->QueueFamily;
		barrier.image = _image;

		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		switch (from)
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
		default:
			LogError(_device->VulkanPlatform->GetLogger(), FormattedString("Transitioning from {} is unsupported",
				string_VkImageLayout(from)
			));
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
		default:
			LogError(_device->VulkanPlatform->GetLogger(), FormattedString("Transitioning to {} is unsupported",
				string_VkImageLayout(to)
			));
			return;
		}

		vkCmdPipelineBarrier(
			commandBuffer->GetCmdBuffer(),
			sourceStage,
			destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);
	}

	void ImageVulkan::CreateImageFromDescription()
	{
		VkImageCreateInfo create = {};
		create.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		create.extent.width = static_cast<uint32_t>(Description.Width);
		create.extent.height = static_cast<uint32_t>(Description.Height);
		create.extent.depth = static_cast<uint32_t>(Description.Depth);
		create.mipLevels = static_cast<uint32_t>(Description.MipCount);
		create.arrayLayers = static_cast<uint32_t>(Description.Layers);
		create.imageType = ToVkImageType(Description.DimensionType);
		create.format = ToVkFormat(Description.PixelFormat);
		create.tiling = VK_IMAGE_TILING_OPTIMAL;
		create.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		create.usage = ToVkImageUsageFlags(Description.UsageFlags);
		create.samples = VK_SAMPLE_COUNT_1_BIT; // TODO: multiple samples
		create.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // TODO: configurable sharing mode

		AssertVkResult(vkCreateImage(_device->GetDevice(), &create, nullptr, &_image));

		// Query memory requirements
		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements(_device->GetDevice(), _image, &memoryRequirements);

		if (!_device->FindMemoryIndex(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _memoryIndex))
			throw VulkanRenderingException("Unable to find local memory for image");

		// Allocate memory
		VkMemoryAllocateInfo allocateInfo = {};
		allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocateInfo.allocationSize = memoryRequirements.size;
		allocateInfo.memoryTypeIndex = _memoryIndex;

		AssertVkResult(vkAllocateMemory(_device->GetDevice(), &allocateInfo, nullptr, &_imageMemory));

		// Bind image memory
		AssertVkResult(vkBindImageMemory(_device->GetDevice(), _image, _imageMemory, 0)); // TODO: configurable memory offset
	}

	void ImageVulkan::CreateNativeImageView()
	{
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.format = ToVkFormat(Description.PixelFormat);
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = static_cast<uint32_t>(Description.MipCount);
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = static_cast<uint32_t>(Description.Layers);
		createInfo.image = _image;

		AssertVkResult(vkCreateImageView(_device->GetDevice(), &createInfo, nullptr, &_nativeView));
	}
}