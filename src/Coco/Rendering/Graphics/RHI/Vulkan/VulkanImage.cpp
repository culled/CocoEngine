#include "Renderpch.h"
#include "VulkanImage.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanCommandBuffer.h"
#include "VulkanBuffer.h"

#include "VulkanUtils.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	VulkanImageInfo::VulkanImageInfo() :
		VulkanImageInfo(nullptr)
	{}

	VulkanImageInfo::VulkanImageInfo(VkImage image) :
		Image(image),
		Memory(nullptr),
		AllocInfo(),
		CurrentLayout(VK_IMAGE_LAYOUT_UNDEFINED)
	{}

	VulkanImage::VulkanImage(const GraphicsResourceID& id, VulkanGraphicsDevice& device, const ImageDescription& description, VkImage image) :
		Image(id),
		_device(device),
		_description(description),
		_imageInfo(image)
	{
		CreateNativeImageView(_device, _description, _imageInfo);
	}

	VulkanImage::VulkanImage(const GraphicsResourceID& id, VulkanGraphicsDevice& device, const ImageDescription& description) :
		Image(id),
		_device(device),
		_description(description),
		_imageInfo()
	{
		CreateImage(_device, description, false, _imageInfo);
		CreateNativeImageView(_device, _description, _imageInfo);
	}

	VulkanImage::~VulkanImage()
	{
		DestroyNativeImageView(_device, _imageInfo);
		DestroyImage(_device, _imageInfo);
	}

	void VulkanImage::SetPixels(uint64 offset, const void* pixelData, uint64 pixelDataSize)
	{
		VulkanQueue* queue = _device.GetQueue(VulkanQueueType::Graphics);
		if (!queue)
			throw InvalidOperationException("A graphics queue is required to transfer pixel data");

		uint64 imageSize = GetDataSize();

		if (offset + pixelDataSize > imageSize)
		{
			throw OutOfRangeException(
				FormatString("The combination of offset and size must fall within the image memory. 0 < {} <= {}",
					offset + pixelDataSize,
					imageSize
				)
			);
		}

		Ref<VulkanBuffer> stagingBuffer = StaticRefCast<VulkanBuffer>(Buffer::CreateStaging(_device, pixelDataSize));

		stagingBuffer->LoadData(0, pixelData, pixelDataSize);

		VulkanCommandBuffer buffer = queue->GetCommandPool()->Allocate(true);
		buffer.Begin(true, false);

		TransitionLayout(buffer, *queue, *queue, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		CopyFromBuffer(buffer, *stagingBuffer);

		if (_description.MipCount > 1)
		{
			GenerateMipMaps(buffer);
		}

		TransitionLayout(buffer, *queue, *queue, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		buffer.EndAndSubmit();
		queue->WaitForIdle();
		queue->GetCommandPool()->Free(buffer);

		_device.TryReleaseResource(stagingBuffer->ID);
	}

	void VulkanImage::TransitionLayout(const VulkanCommandBuffer& commandBuffer, VulkanQueue& fromQueue, VulkanQueue& toQueue, VkImageLayout to)
	{
		TransitionLayout(_device, commandBuffer, fromQueue, toQueue, to, _description, _imageInfo);
	}

	void VulkanImage::AdjustDescription(VulkanGraphicsDevice& device, ImageDescription& description)
	{
		// Adjust the description to fit within the device's capabilities
		const GraphicsDeviceFeatures& features = device.GetFeatures();
		description.SampleCount = static_cast<MSAASamples>(Math::Min(features.MaximumMSAASamples, description.SampleCount));
		description.Width = Math::Min(description.Width, features.MaxImageWidth);
		description.Height = Math::Min(description.Height, features.MaxImageHeight);
		description.Depth = Math::Min(description.Depth, features.MaxImageDepth);
	}

	void VulkanImage::CreateImage(VulkanGraphicsDevice& device, const ImageDescription& description, bool enableHostVisible, VulkanImageInfo& outImageInfo)
	{
		ImageUsageFlags usageFlags = description.UsageFlags;
		bool hostVisible = (usageFlags & ImageUsageFlags::HostVisible) == ImageUsageFlags::HostVisible && enableHostVisible;

		if (!hostVisible)
		{
			usageFlags |= ImageUsageFlags::TransferSource | ImageUsageFlags::TransferDestination;
			usageFlags &= ~ImageUsageFlags::HostVisible;
		}

		VkImageCreateInfo create{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		create.extent.width = description.Width;
		create.extent.height = description.Height;
		create.extent.depth = description.Depth;
		create.mipLevels = description.MipCount;
		create.arrayLayers = description.Layers;
		create.imageType = ToVkImageType(description.DimensionType);
		create.format = ToVkFormat(description.PixelFormat, description.ColorSpace);
		create.tiling = hostVisible ? VK_IMAGE_TILING_LINEAR : VK_IMAGE_TILING_OPTIMAL;
		create.initialLayout = outImageInfo.CurrentLayout;
		create.usage = ToVkImageUsageFlags(usageFlags, description.PixelFormat);
		create.samples = ToVkSampleCountFlagBits(description.SampleCount);
		create.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // TODO: configurable sharing mode

		VmaAllocationCreateInfo allocCreateInfo{};
		allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
		allocCreateInfo.flags = 0;

		if (hostVisible)
		{
			allocCreateInfo.flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
		}

		AssertVkSuccess(
			vmaCreateImage(
				device.GetAllocator(),
				&create,
				&allocCreateInfo,
				&outImageInfo.Image,
				&outImageInfo.Memory,
				&outImageInfo.AllocInfo
			)
		)
	}

	void VulkanImage::DestroyImage(VulkanGraphicsDevice& device, VulkanImageInfo& imageInfo)
	{
		if (imageInfo.Image && imageInfo.Memory)
		{
			device.WaitForIdle();

			vmaDestroyImage(device.GetAllocator(), imageInfo.Image, imageInfo.Memory);

			imageInfo.Memory = nullptr;
			imageInfo.Image = nullptr;
		}
	}

	void VulkanImage::CreateNativeImageView(VulkanGraphicsDevice& device, const ImageDescription& description, VulkanImageInfo& imageInfo)
	{
		const bool isDepthFormat = IsDepthFormat(description.PixelFormat);
		const bool isStencilFormat = IsStencilFormat(description.PixelFormat);
		const bool isColorFormat = !isDepthFormat && !isStencilFormat;

		VkImageViewCreateInfo createInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		createInfo.format = ToVkFormat(description.PixelFormat, description.ColorSpace);
		createInfo.viewType = ToVkImageViewType(description.DimensionType);

		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		if (isColorFormat)
			createInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_COLOR_BIT;
		
		if (isDepthFormat)
			createInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;

		if (isStencilFormat)
			createInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = static_cast<uint32_t>(description.MipCount);
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = static_cast<uint32_t>(description.Layers);
		createInfo.image = imageInfo.Image;

		AssertVkSuccess(vkCreateImageView(device.GetDevice(), &createInfo, device.GetAllocationCallbacks(), &imageInfo.NativeView))
	}

	void VulkanImage::DestroyNativeImageView(VulkanGraphicsDevice& device, VulkanImageInfo& imageInfo)
	{
		if (!imageInfo.NativeView)
			return;

		device.WaitForIdle();
		vkDestroyImageView(device.GetDevice(), imageInfo.NativeView, device.GetAllocationCallbacks());
		imageInfo.NativeView = nullptr;
	}

	void VulkanImage::TransitionLayout(
		VulkanGraphicsDevice& device, 
		const VulkanCommandBuffer& commandBuffer, 
		VulkanQueue& fromQueue, VulkanQueue& toQueue,
		VkImageLayout to,
		const ImageDescription& description,
		VulkanImageInfo& imageData)
	{
		if (imageData.CurrentLayout == to || to == VK_IMAGE_LAYOUT_UNDEFINED)
			return;

		VkImageMemoryBarrier barrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		barrier.oldLayout = imageData.CurrentLayout;
		barrier.newLayout = to;
		barrier.srcQueueFamilyIndex = fromQueue.GetFamilyIndex();
		barrier.dstQueueFamilyIndex = toQueue.GetFamilyIndex();
		barrier.image = imageData.Image;

		VkImageAspectFlags aspectFlags = 0;

		if (IsDepthFormat(description.PixelFormat) || IsStencilFormat(description.PixelFormat))
		{
			if (IsStencilFormat(description.PixelFormat))
				aspectFlags |= VK_IMAGE_ASPECT_STENCIL_BIT;

			if (IsDepthFormat(description.PixelFormat))
				aspectFlags |= VK_IMAGE_ASPECT_DEPTH_BIT;
		}
		else
		{
			aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
		}

		barrier.subresourceRange.aspectMask = aspectFlags;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = description.MipCount;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = description.Layers;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		switch (imageData.CurrentLayout)
		{
		case VK_IMAGE_LAYOUT_UNDEFINED:
		case VK_IMAGE_LAYOUT_GENERAL:
		{
			barrier.srcAccessMask = 0;
			sourceStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		{
			barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
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
			sourceStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			break;
		}
		default:
			CocoError("Transitioning from {} is unsupported", string_VkImageLayout(imageData.CurrentLayout));
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
		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		{
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		{
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			destinationStage = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
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
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		{
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			destinationStage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_GENERAL:
		{
			barrier.dstAccessMask = VK_ACCESS_HOST_READ_BIT | VK_ACCESS_MEMORY_READ_BIT;
			destinationStage = VK_PIPELINE_STAGE_HOST_BIT;
			break;
		}
		default:
			CocoError("Transitioning to {} is not supported", string_VkImageLayout(to))
				return;
		}

		imageData.CurrentLayout = to;

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

	void VulkanImage::CopyFromBuffer(const VulkanCommandBuffer& commandBuffer, const VulkanBuffer& source)
	{
		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageExtent.width = _description.Width;
		region.imageExtent.height = _description.Height;
		region.imageExtent.depth = _description.Depth;

		vkCmdCopyBufferToImage(commandBuffer.GetCmdBuffer(), source.GetBuffer(), _imageInfo.Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		VulkanQueue* graphicsQueue = _device.GetQueue(VulkanQueueType::Graphics);
		CocoAssert(graphicsQueue, "Device does not support graphics operations")

		VkImageLayout oldLayout = _imageInfo.CurrentLayout;
		_imageInfo.CurrentLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		TransitionLayout(commandBuffer, *graphicsQueue, *graphicsQueue, oldLayout);
	}

	void VulkanImage::GenerateMipMaps(const VulkanCommandBuffer& commandBuffer)
	{
		VulkanQueue* graphicsQueue = _device.GetQueue(VulkanQueueType::Graphics);
		CocoAssert(graphicsQueue, "Device does not support graphics operations")

		if (_imageInfo.CurrentLayout != VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
			TransitionLayout(commandBuffer, *graphicsQueue, *graphicsQueue, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		VkImageMemoryBarrier barrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		barrier.image = _imageInfo.Image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;

		int32 mipWidth = static_cast<int32>(_description.Width);
		int32 mipHeight = static_cast<int32>(_description.Height);

		for (uint32 m = 1; m < _description.MipCount; m++)
		{
			barrier.subresourceRange.baseMipLevel = m - 1;

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer.GetCmdBuffer(),
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier
			);

			VkImageBlit blit{};
			blit.srcOffsets[0] = { 0, 0, 0 };
			blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };

			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel = m - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;

			int32 dstMipWidth = mipWidth > 1 ? mipWidth / 2 : mipWidth;
			int32 dstMipHeight = mipHeight > 1 ? mipHeight / 2 : mipHeight;
			blit.dstOffsets[0] = { 0, 0, 0 };
			blit.dstOffsets[1] = { dstMipWidth, dstMipHeight, 1 };
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel = m;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;

			vkCmdBlitImage(commandBuffer.GetCmdBuffer(),
				_imageInfo.Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				_imageInfo.Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &blit,
				VK_FILTER_LINEAR
			);

			mipWidth = dstMipWidth;
			mipHeight = dstMipHeight;
		}

		// Transition the last mip level
		barrier.subresourceRange.baseMipLevel = _description.MipCount - 1;

		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		
		vkCmdPipelineBarrier(commandBuffer.GetCmdBuffer(),
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		_imageInfo.CurrentLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

		TransitionLayout(commandBuffer, *graphicsQueue, *graphicsQueue, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}
}