//
// Created by cullen on 3/22/26.
//

#include "../VulkanGraphicsPlatform.h"
#include "Coco/Core/Engine.h"
#include "VulkanImage.h"
#include "../VulkanUtils.h"
#include "../VulkanStagingBuffer.h"
#include "Coco/Rendering/RHI/Vulkan/VulkanQueue.h"

namespace Coco
{
    VulkanImageInfo::VulkanImageInfo() :
        Image(nullptr),
        NativeView(nullptr),
        DepthView(nullptr),
        Memory(nullptr),
        AllocInfo(),
        CurrentLayout(VK_IMAGE_LAYOUT_UNDEFINED)
    {}

    VulkanImage::VulkanImage(uint64 id, VulkanGraphicsPlatform* platform, const ImageDescription& description) :
        Image(id, description),
        _platform(platform),
        _imageInfo()
		//_lastOperationQueue(platform->GetQueue(VulkanQueue::Type::Graphics))
    {
    	CreateImage(description, _imageInfo);
    	CreateNativeView(description, _imageInfo);
    }

    VulkanImage::VulkanImage(uint64 id, VulkanGraphicsPlatform* platform, const ImageDescription& description,
        VkImage image) :
		Image(id, description),
        _platform(platform),
		_imageInfo()
		//_lastOperationQueue(platform->GetQueue(VulkanQueue::Type::Graphics))
    {
        _imageInfo.Image = image;

        CreateNativeView(description, _imageInfo);

    	COCO_ENGINE_LOG_VERBOSE("Created VulkanImage %u from existing image", id);
    }

    VulkanImage::~VulkanImage()
    {
        DestroyImage(_imageInfo);

    	COCO_ENGINE_LOG_VERBOSE("Destroyed VulkanImage %u", GetID());
    }

    void VulkanImage::SetPixels(const void* pixelData, uint64 pixelDataSize)
    {
		uint64 dataSize = Math::Min(GetPixelDataSize(), pixelDataSize);

    	VulkanStagingBuffer* stagingBuffer = static_cast<VulkanStagingBuffer*>(_platform->GetStagingBuffer());
		VulkanStagingOperation* stagingOperation = static_cast<VulkanStagingOperation*>(stagingBuffer->CreateStagingOperation(dataSize));
    	memcpy(stagingOperation->BufferPtr, pixelData, dataSize);

    	TransitionLayout(stagingOperation->CommandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    	VkImageSubresourceRange subresourceRange = {
    		VulkanUtils::ToVkImageAspectFlags(_description.AttachmentType),
    		0, 1,
    		0, 1
    	};

    	VkBufferImageCopy region{};
    	region.bufferOffset = stagingOperation->BufferOffset;
    	region.bufferRowLength = 0;
    	region.bufferImageHeight = 0;

    	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    	region.imageSubresource.mipLevel = 0;
    	region.imageSubresource.baseArrayLayer = 0;
    	region.imageSubresource.layerCount = 1;

    	region.imageExtent.width = _description.Width;
    	region.imageExtent.height = _description.Height;
    	region.imageExtent.depth = _description.Depth;

    	VulkanBuffer* buffer = static_cast<VulkanBuffer*>(stagingOperation->StagingBuffer.Get());
    	vkCmdCopyBufferToImage(stagingOperation->CommandBuffer, buffer->GetBuffer(), _imageInfo.Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		if (_description.MipCount > 1)
		{
    		stagingBuffer->AddImageLayoutTransitionBarrier(_imageInfo.Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange);
			GenerateMipMaps(stagingBuffer->GetCurrentGraphicsCommandBuffer());
		}
    	else
    	{
    		stagingBuffer->AddImageLayoutTransitionBarrier(_imageInfo.Image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourceRange);
    		_imageInfo.CurrentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    	}
    }

    void VulkanImage::TransitionLayout(VkCommandBuffer commandBuffer, VkImageLayout newLayout)
    {
    	if (_imageInfo.CurrentLayout == newLayout || newLayout == VK_IMAGE_LAYOUT_UNDEFINED)
    		return;

    	VkImageSubresourceRange subresourceRange = {
    		VulkanUtils::ToVkImageAspectFlags(_description.AttachmentType),
			0, _description.MipCount,
			0, _description.Layers
		};

    	VkPipelineStageFlags2 sourceStage;
    	VkAccessFlagBits2 srcAccessMask;
    	VkPipelineStageFlags2 destinationStage;
    	VkAccessFlagBits2 dstAccessMask;

    	switch (_imageInfo.CurrentLayout)
		{
		case VK_IMAGE_LAYOUT_UNDEFINED:
		case VK_IMAGE_LAYOUT_GENERAL:
		{
			srcAccessMask = 0;
			sourceStage = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		{
			srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
			sourceStage = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		{
			srcAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT;
			sourceStage = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
		{
			srcAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
			sourceStage = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		{
			srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
			sourceStage = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
		case VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL:
		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		{
			srcAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			sourceStage = VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
		{
			srcAccessMask = 0;
			sourceStage = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
			break;
		}
		default:
			COCO_ENGINE_LOG_ERROR("Transitioning from %u is unsupported", _imageInfo.CurrentLayout);
			return;
		}

		switch (newLayout)
		{
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		{
			dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
			destinationStage = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		{
			dstAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT;
			destinationStage = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
		{
			dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
			destinationStage = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
		{
			dstAccessMask = 0;
			destinationStage = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		{
			dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
			destinationStage = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
		case VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL:
		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		{
			dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			destinationStage = VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_GENERAL:
		{
			dstAccessMask = VK_ACCESS_2_HOST_READ_BIT | VK_ACCESS_2_MEMORY_READ_BIT;
			destinationStage = VK_PIPELINE_STAGE_2_HOST_BIT;
			break;
		}
		default:
			COCO_ENGINE_LOG_ERROR("Transitioning to %u is not supported", newLayout);
			return;
		}

    	VkImageMemoryBarrier2 memoryBarrier = {
    		VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
			nullptr,
			sourceStage, srcAccessMask,
			destinationStage, dstAccessMask,
			_imageInfo.CurrentLayout, newLayout,
			VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
			_imageInfo.Image,
			subresourceRange
		};

    	VkDependencyInfo dependencyInfo = {
    		VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
			nullptr,
			0,
			0, nullptr,
			0, nullptr,
			1, &memoryBarrier,
		};

    	vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
    	_imageInfo.CurrentLayout = newLayout;
    }

    void VulkanImage::CreateImage(const ImageDescription& description, VulkanImageInfo& outImageInfo)
    {
    	ImageUsageFlags usageFlags = description.UsageFlags | ImageUsageFlags::TransferSource | ImageUsageFlags::TransferDestination;
    	bool hostVisible = (usageFlags & ImageUsageFlags::HostVisible) == ImageUsageFlags::HostVisible;

    	VkImageCreateInfo create{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
    	create.extent.width = description.Width;
    	create.extent.height = description.Height;
    	create.extent.depth = description.Depth;
    	create.mipLevels = description.MipCount;
    	create.arrayLayers = description.Layers;
    	create.imageType = VulkanUtils::ToVkImageType(description.Height, description.Depth);
    	create.format = VulkanUtils::ToVkFormat(description.PixelFormat, description.ColorSpace);
    	create.tiling = hostVisible ? VK_IMAGE_TILING_LINEAR : VK_IMAGE_TILING_OPTIMAL;
    	create.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    	create.usage = VulkanUtils::ToVkImageUsageFlags(usageFlags, description.AttachmentType);
    	create.samples = VulkanUtils::ToVkSampleFlags(description.SampleCount);
    	create.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    	VmaAllocationCreateInfo allocCreateInfo{};
    	allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
    	allocCreateInfo.flags = 0;

    	if (hostVisible)
    		allocCreateInfo.flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

    	AssertVkSuccess(
			vmaCreateImage(
				_platform->GetVmaAllocator(),
				&create,
				&allocCreateInfo,
				&outImageInfo.Image,
				&outImageInfo.Memory,
				&outImageInfo.AllocInfo
			)
		);
    }

    void VulkanImage::CreateNativeView(const ImageDescription& description, VulkanImageInfo& outImageInfo)
    {
        VkImageViewCreateInfo createInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        createInfo.format = VulkanUtils::ToVkFormat(description.PixelFormat, description.ColorSpace);
        createInfo.viewType = VulkanUtils::ToVkImageViewType(description.Height, description.Depth, description.Layers);

        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask = VulkanUtils::ToVkImageAspectFlags(_description.AttachmentType);

        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = static_cast<uint32_t>(description.MipCount);
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = static_cast<uint32_t>(description.Layers);
        createInfo.image = outImageInfo.Image;

        AssertVkSuccess(
            vkCreateImageView(
                _platform->GetDevice(),
                &createInfo,
                _platform->GetAllocationCallbacks(),
                &outImageInfo.NativeView
            )
        );
    }

    void VulkanImage::GenerateMipMaps(VkCommandBuffer commandBuffer)
    {
		TransitionLayout(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

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

			vkCmdPipelineBarrier(commandBuffer,
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

			vkCmdBlitImage(commandBuffer,
				_imageInfo.Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				_imageInfo.Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &blit,
				VK_FILTER_LINEAR
			);

			mipWidth = dstMipWidth;
			mipHeight = dstMipHeight;

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);
		}

		// Transition the last mip level
		barrier.subresourceRange.baseMipLevel = _description.MipCount - 1;

		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		_imageInfo.CurrentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    	COCO_ENGINE_LOG_VERBOSE("Created %u mipmaps for VulkanImage %u", _description.MipCount, GetID());
    }

    void VulkanImage::DestroyImage(VulkanImageInfo& imageInfo)
    {
        _platform->WaitForIdle();

        if (imageInfo.NativeView)
        {
            vkDestroyImageView(_platform->GetDevice(), imageInfo.NativeView, _platform->GetAllocationCallbacks());
            imageInfo.NativeView = nullptr;
        }

        if (imageInfo.DepthView)
        {
            vkDestroyImageView(_platform->GetDevice(), imageInfo.DepthView, _platform->GetAllocationCallbacks());
            imageInfo.DepthView = nullptr;
        }

        if (imageInfo.Image && imageInfo.Memory)
        {
            vmaDestroyImage(_platform->GetVmaAllocator(), imageInfo.Image, imageInfo.Memory);
            imageInfo.Image = nullptr;
            imageInfo.Memory = nullptr;
        }
    }
} // Coco