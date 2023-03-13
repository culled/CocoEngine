#include "ImageVulkan.h"

#include "GraphicsDeviceVulkan.h"
#include "GraphicsPlatformVulkan.h"
#include "BufferVulkan.h"
#include "CommandBufferVulkan.h"

#include "VulkanUtilities.h"

namespace Coco::Rendering
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
	}

	ImageVulkan::~ImageVulkan()
	{
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

	void ImageVulkan::SetPixelData(const void* pixelData)
	{
		CommandBufferPoolVulkan* pool;
		if (!_device->GetGraphicsCommandPool(&pool))
			throw Exception("A graphics queue is required to upload pixel data");

		uint64_t bufferSize = static_cast<uint64_t>(Description.Width) *
			static_cast<uint64_t>(Description.Height) *
			static_cast<uint64_t>(Description.Depth) *
			static_cast<uint64_t>(GetPixelFormatSize(Description.PixelFormat));

		GraphicsResourceRef<BufferVulkan> staging = _device->CreateResource<BufferVulkan>(
			BufferUsageFlags::HostVisible | BufferUsageFlags::TransferSource,
			bufferSize,
			0,
			true);

		staging->LoadData(0, bufferSize, pixelData);

		CommandBufferVulkan* commandBuffer = static_cast<CommandBufferVulkan*>(pool->Allocate(true));
		commandBuffer->Begin(true, false);

		TransitionLayout(commandBuffer, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		CopyFromBuffer(commandBuffer, staging.get());
		TransitionLayout(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		commandBuffer->EndAndSubmit();
		pool->WaitForQueue();
		pool->Free(commandBuffer);

		staging.reset();
	}

	void ImageVulkan::CopyFromBuffer(CommandBufferVulkan* commandBuffer, BufferVulkan* buffer)
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
		region.imageExtent.depth = 1;

		vkCmdCopyBufferToImage(commandBuffer->GetCmdBuffer(), buffer->GetBuffer(), _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	}

	void ImageVulkan::TransitionLayout(CommandBufferVulkan* commandBuffer, VkImageLayout from, VkImageLayout to)
	{
		Ref<VulkanQueue> graphicsQueue;

		if (!_device->GetComputeQueue(graphicsQueue))
			throw Exception("Device needs a graphics queue to transition image layouts");

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

		if (from == VK_IMAGE_LAYOUT_UNDEFINED && to == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			// Don't care about the old layout, so transition to optimal layout for the underlying implementation
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			// Start copying from any stage
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (from == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && to == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			// Transitioning from a transfer destination to a shader read-only layout
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			// Start copying from the transfer to the fragment stage
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else
		{
			LogError(_device->VulkanPlatform->GetLogger(), 
				FormattedString("Transitioning from {} to {} is unsupported currently", 
					string_VkImageLayout(from), 
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
			throw Exception("Unable to find memory type for image");

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