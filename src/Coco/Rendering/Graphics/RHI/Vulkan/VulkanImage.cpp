#include "Renderpch.h"
#include "VulkanImage.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanCommandBuffer.h"
#include "VulkanUtils.h"
#include "VulkanBuffer.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	VulkanImage::VulkanImage(const GraphicsDeviceResourceID& id, const ImageDescription& description) :
		GraphicsDeviceResource<VulkanGraphicsDevice>(id),
		_image(nullptr),
		_isManagedExternally(false),
		_description(description),
		_memoryIndex(0),
		_imageMemory(nullptr),
		_currentLayout(VK_IMAGE_LAYOUT_UNDEFINED)
	{
		// Adjust the description to fit within the device's capabilities
		const GraphicsDeviceFeatures& features = _device.GetFeatures();
		_description.SampleCount = static_cast<MSAASamples>(Math::Min(features.MaximumMSAASamples, _description.SampleCount));
		_description.Width = Math::Min(_description.Width, features.MaxImageWidth);
		_description.Height = Math::Min(_description.Height, features.MaxImageHeight);
		_description.Depth = Math::Min(_description.Depth, features.MaxImageDepth);

		CreateImage();
		CreateNativeImageView();
	}

	VulkanImage::VulkanImage(const GraphicsDeviceResourceID& id, const ImageDescription& description, VkImage image) :
		GraphicsDeviceResource<VulkanGraphicsDevice>(id),
		_image(image),
		_isManagedExternally(true),
		_description(description),
		_memoryIndex(0),
		_imageMemory(nullptr),
		_currentLayout(VK_IMAGE_LAYOUT_UNDEFINED)
	{
		CreateNativeImageView();
	}

	VulkanImage::~VulkanImage()
	{
		_device.WaitForIdle();

		if (_nativeView)
		{
			vkDestroyImageView(_device.GetDevice(), _nativeView, _device.GetAllocationCallbacks());
			_nativeView = nullptr;
		}

		if (_image && !_isManagedExternally)
		{
			vkFreeMemory(_device.GetDevice(), _imageMemory, _device.GetAllocationCallbacks());
			vkDestroyImage(_device.GetDevice(), _image, _device.GetAllocationCallbacks());

			_imageMemory = nullptr;
			_image = nullptr;
		}
	}

	uint64 VulkanImage::GetSize() const
	{
		return _description.Width * _description.Height * _description.Depth * GetPixelFormatSize(_description.PixelFormat);
	}

	void VulkanImage::SetPixels(uint64 offset, const void* pixelData, uint64 pixelDataSize)
	{
		DeviceQueue* queue = _device.GetQueue(DeviceQueue::Type::Graphics);
		if (!queue)
			throw std::exception("A graphics queue is required to transfer pixel data");

		uint64 imageSize = GetSize();

		if (offset + pixelDataSize > imageSize)
		{
			string err = FormatString("The combination of offset and size must fall within the image memory. 0 < {} <= {}",
				offset + pixelDataSize,
				imageSize
			);

			throw std::exception(err.c_str());
		}

		Ref<VulkanBuffer> staging = _device.CreateBuffer(
			pixelDataSize,
			BufferUsageFlags::HostVisible | BufferUsageFlags::TransferSource | BufferUsageFlags::TransferDestination,
			true);

		staging->LoadData(0, pixelData, pixelDataSize);

		UniqueRef<VulkanCommandBuffer> buffer = queue->Pool.Allocate(true);
		buffer->Begin(true, false);

		TransitionLayout(*buffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		CopyFromBuffer(*buffer, *staging);
		TransitionLayout(*buffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		buffer->EndAndSubmit();
		_device.WaitForQueueIdle(DeviceQueue::Type::Graphics);
		queue->Pool.Free(*buffer);

		staging.Invalidate();
		_device.PurgeUnusedResources();
	}

	void VulkanImage::TransitionLayout(VulkanCommandBuffer& commandBuffer, VkImageLayout to)
	{
		if (_currentLayout == to || to == VK_IMAGE_LAYOUT_UNDEFINED)
			return;

		DeviceQueue* graphicsQueue = _device.GetQueue(DeviceQueue::Type::Graphics);
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

	void VulkanImage::CreateImage()
	{
		VkImageCreateInfo create{};
		create.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		create.extent.width = _description.Width;
		create.extent.height = _description.Height;
		create.extent.depth = _description.Depth;
		create.mipLevels = _description.MipCount;
		create.arrayLayers = _description.Layers;
		create.imageType = ToVkImageType(_description.DimensionType);
		create.format = ToVkFormat(_description.PixelFormat, _description.ColorSpace);
		create.tiling = VK_IMAGE_TILING_OPTIMAL;
		create.initialLayout = _currentLayout;
		create.usage = ToVkImageUsageFlags(_description.UsageFlags, _description.PixelFormat);
		create.samples = ToVkSampleCountFlagBits(_description.SampleCount);
		create.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // TODO: configurable sharing mode

		AssertVkSuccess(vkCreateImage(_device.GetDevice(), &create, _device.GetAllocationCallbacks(), &_image));

		// Query memory requirements
		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements(_device.GetDevice(), _image, &memoryRequirements);

		if (!_device.FindMemoryIndex(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _memoryIndex))
			throw std::exception("Unable to find memory for image");

		// Allocate memory
		VkMemoryAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocateInfo.allocationSize = memoryRequirements.size;
		allocateInfo.memoryTypeIndex = _memoryIndex;

		AssertVkSuccess(vkAllocateMemory(_device.GetDevice(), &allocateInfo, _device.GetAllocationCallbacks(), &_imageMemory));

		// Bind image memory
		AssertVkSuccess(vkBindImageMemory(_device.GetDevice(), _image, _imageMemory, 0));
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

		AssertVkSuccess(vkCreateImageView(_device.GetDevice(), &createInfo, _device.GetAllocationCallbacks(), &_nativeView));
	}

	void VulkanImage::CopyFromBuffer(VulkanCommandBuffer& commandBuffer, VulkanBuffer& source)
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
		region.imageExtent.height =_description.Height;
		region.imageExtent.depth = _description.Depth;

		vkCmdCopyBufferToImage(commandBuffer.GetCmdBuffer(), source.GetBuffer(), _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	}
}