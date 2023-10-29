#include "Renderpch.h"
#include "VulkanImage.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanCommandBuffer.h"
#include "VulkanUtils.h"
#include "VulkanBuffer.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	VulkanImageData::VulkanImageData() :
		Image(nullptr),
		Memory(nullptr),
		HeapIndex(0),
		CurrentLayout(VK_IMAGE_LAYOUT_UNDEFINED)
	{}

	VulkanImageData::VulkanImageData(VkImage image) :
		Image(image),
		Memory(nullptr),
		HeapIndex(0),
		CurrentLayout(VK_IMAGE_LAYOUT_UNDEFINED)
	{}

	VulkanImage::VulkanImage(const GraphicsDeviceResourceID& id, const ImageDescription& description) :
		GraphicsDeviceResource<VulkanGraphicsDevice>(id),
		_description(description),
		_imageData(),
		_hostImageData()
	{
		// Adjust the description to fit within the device's capabilities
		const GraphicsDeviceFeatures& features = _device.GetFeatures();
		_description.SampleCount = static_cast<MSAASamples>(Math::Min(features.MaximumMSAASamples, _description.SampleCount));
		_description.Width = Math::Min(_description.Width, features.MaxImageWidth);
		_description.Height = Math::Min(_description.Height, features.MaxImageHeight);
		_description.Depth = Math::Min(_description.Depth, features.MaxImageDepth);

		// Force transfer source if we're host visible
		if ((_description.UsageFlags & ImageUsageFlags::HostVisible) == ImageUsageFlags::HostVisible)
			_description.UsageFlags |= ImageUsageFlags::TransferSource;

		CreateImage(false, _imageData);
		CreateNativeImageView();

		if ((_description.UsageFlags & ImageUsageFlags::HostVisible) == ImageUsageFlags::HostVisible)
			CreateImage(true, _hostImageData);
	}

	VulkanImage::VulkanImage(const GraphicsDeviceResourceID& id, const ImageDescription& description, VkImage image) :
		GraphicsDeviceResource<VulkanGraphicsDevice>(id),
		_description(description),
		_imageData(VulkanImageData(image)),
		_hostImageData()
	{
		CreateNativeImageView();

		if ((_description.UsageFlags & ImageUsageFlags::HostVisible) == ImageUsageFlags::HostVisible)
			CreateImage(true, _hostImageData);
	}

	VulkanImage::~VulkanImage()
	{
		_device.WaitForIdle();

		if (_nativeView)
		{
			vkDestroyImageView(_device.GetDevice(), _nativeView, _device.GetAllocationCallbacks());
			_nativeView = nullptr;
		}

		DestroyImage(_imageData);
		DestroyImage(_hostImageData);
	}

	uint64 VulkanImage::GetDataSize() const
	{
		return static_cast<uint64>(_description.Width) * _description.Height * _description.Depth * GetPixelFormatSize(_description.PixelFormat);
	}

	void VulkanImage::SetPixels(uint64 offset, const void* pixelData, uint64 pixelDataSize)
	{
		DeviceQueue* queue = _device.GetQueue(DeviceQueue::Type::Graphics);
		if (!queue)
			throw std::exception("A graphics queue is required to transfer pixel data");

		uint64 imageSize = GetDataSize();

		if (offset + pixelDataSize > imageSize)
		{
			string err = FormatString("The combination of offset and size must fall within the image memory. 0 < {} <= {}",
				offset + pixelDataSize,
				imageSize
			);

			throw std::exception(err.c_str());
		}

		ManagedRef<VulkanBuffer> staging = CreateManagedRef<VulkanBuffer>(
			0,
			pixelDataSize,
			BufferUsageFlags::HostVisible | BufferUsageFlags::TransferSource | BufferUsageFlags::TransferDestination,
			true);

		staging->LoadData(0, pixelData, pixelDataSize);

		UniqueRef<VulkanCommandBuffer> buffer = queue->Pool.Allocate(true);
		buffer->Begin(true, false);

		TransitionLayout(*buffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		CopyFromBuffer(*buffer, *staging);

		if (_description.MipCount > 1)
		{
			GenerateMipMaps(*buffer);
		}

		TransitionLayout(*buffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		buffer->EndAndSubmit();
		_device.WaitForQueueIdle(DeviceQueue::Type::Graphics);
		queue->Pool.Free(*buffer);

		staging.Invalidate();
	}

	void VulkanImage::ReadPixel(const Vector2Int& pixelCoords, void* outData, size_t dataSize)
	{
		if ((_description.UsageFlags & ImageUsageFlags::HostVisible) != ImageUsageFlags::HostVisible)
		{
			CocoError("Cannot read from an image that was not created with the ImageUsageFlags::HostVisible flag set")
			return;
		}

		uint8 sourcePixelSize = GetPixelFormatSize(_description.PixelFormat);
		if (dataSize != sourcePixelSize)
		{
			CocoError("Data size does not match the pixel data size: Data Size: {}, Pixel Data Size: {}", dataSize, sourcePixelSize)
			return;
		}

		Copy(*this, RectInt(Vector2Int::Zero, Vector2Int(_description.Width, _description.Height)), _hostImageData, VK_IMAGE_LAYOUT_GENERAL);

		// Get layout of the image (including row pitch)
		VkImageSubresource subResource{};
		subResource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		VkSubresourceLayout subResourceLayout;

		vkGetImageSubresourceLayout(_device.GetDevice(), _hostImageData.Image, &subResource, &subResourceLayout);

		// Map the image's data
		uint8* data = nullptr;
		vkMapMemory(_device.GetDevice(), _hostImageData.Memory, 0, VK_WHOLE_SIZE, 0, reinterpret_cast<void**>(&data));
		data += subResourceLayout.offset;

		// Get the index of the pixel in the data
		int clampedX = Math::Clamp(pixelCoords.X, 0, static_cast<int>(_description.Width) - 1);
		int clampedY = Math::Clamp(pixelCoords.Y, 0, static_cast<int>(_description.Height) - 1);
		uint64 index = static_cast<uint64>(clampedY) * subResourceLayout.rowPitch + static_cast<uint64>(clampedX) * sourcePixelSize;

		// Copy the pixel data
		Assert(memcpy_s(outData, dataSize, data + index, sourcePixelSize) == 0)

		vkUnmapMemory(_device.GetDevice(), _hostImageData.Memory);
	}

	void VulkanImage::TransitionLayout(const VulkanCommandBuffer& commandBuffer, VkImageLayout to)
	{
		TransitionLayout(commandBuffer, to, _imageData);
	}

	void VulkanImage::CreateImage(bool hostVisible, VulkanImageData& outImageData)
	{
		ImageUsageFlags usageFlags = _description.UsageFlags;
		
		if (hostVisible)
			usageFlags |= ImageUsageFlags::TransferDestination;
		else
			usageFlags &= ~ImageUsageFlags::HostVisible;

		VkImageCreateInfo create{};
		create.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		create.extent.width = _description.Width;
		create.extent.height = _description.Height;
		create.extent.depth = _description.Depth;
		create.mipLevels = _description.MipCount;
		create.arrayLayers = _description.Layers;
		create.imageType = ToVkImageType(_description.DimensionType);
		create.format = ToVkFormat(_description.PixelFormat, _description.ColorSpace);
		create.tiling = hostVisible ? VK_IMAGE_TILING_LINEAR : VK_IMAGE_TILING_OPTIMAL;
		create.initialLayout = outImageData.CurrentLayout;
		create.usage = ToVkImageUsageFlags(usageFlags, _description.PixelFormat);
		create.samples = ToVkSampleCountFlagBits(_description.SampleCount);
		create.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // TODO: configurable sharing mode

		AssertVkSuccess(vkCreateImage(_device.GetDevice(), &create, _device.GetAllocationCallbacks(), &outImageData.Image));

		// Query memory requirements
		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements(_device.GetDevice(), outImageData.Image, &memoryRequirements);

		VkMemoryPropertyFlags memoryFlags = 0;

		if (hostVisible)
			memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
				VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | 
				(_device.GetFeatures().SupportsHostVisibleLocalMemory ? VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT : 0);
		else
			memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		if (!_device.FindMemoryIndex(memoryRequirements.memoryTypeBits, memoryFlags, outImageData.HeapIndex))
			throw std::exception("Unable to find memory for image");

		// Allocate memory
		VkMemoryAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocateInfo.allocationSize = memoryRequirements.size;
		allocateInfo.memoryTypeIndex = outImageData.HeapIndex;

		AssertVkSuccess(vkAllocateMemory(_device.GetDevice(), &allocateInfo, _device.GetAllocationCallbacks(), &outImageData.Memory));

		// Bind image memory
		AssertVkSuccess(vkBindImageMemory(_device.GetDevice(), outImageData.Image, outImageData.Memory, 0));
	}

	void VulkanImage::DestroyImage(VulkanImageData& imageData)
	{
		if (imageData.Image && imageData.Memory)
		{
			vkFreeMemory(_device.GetDevice(), imageData.Memory, _device.GetAllocationCallbacks());
			vkDestroyImage(_device.GetDevice(), imageData.Image, _device.GetAllocationCallbacks());

			imageData.Memory = nullptr;
			imageData.Image = nullptr;
		}
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
		createInfo.image = _imageData.Image;

		AssertVkSuccess(vkCreateImageView(_device.GetDevice(), &createInfo, _device.GetAllocationCallbacks(), &_nativeView));
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
		region.imageExtent.height =_description.Height;
		region.imageExtent.depth = _description.Depth;

		vkCmdCopyBufferToImage(commandBuffer.GetCmdBuffer(), source.GetBuffer(), _imageData.Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	}

	void VulkanImage::TransitionLayout(const VulkanCommandBuffer& commandBuffer, VkImageLayout to, VulkanImageData& imageData)
	{
		if (imageData.CurrentLayout == to || to == VK_IMAGE_LAYOUT_UNDEFINED)
			return;

		const DeviceQueue* graphicsQueue = _device.GetQueue(DeviceQueue::Type::Graphics);
		if (!graphicsQueue)
			throw std::exception("Device needs a graphics queue to transition image layouts");

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = imageData.CurrentLayout;
		barrier.newLayout = to;
		barrier.srcQueueFamilyIndex = graphicsQueue->FamilyIndex;
		barrier.dstQueueFamilyIndex = graphicsQueue->FamilyIndex;
		barrier.image = imageData.Image;

		VkImageAspectFlags aspectFlags = 0;

		if (IsDepthFormat(_description.PixelFormat) || IsStencilFormat(_description.PixelFormat))
		{
			if (IsStencilFormat(_description.PixelFormat))
				aspectFlags |= VK_IMAGE_ASPECT_STENCIL_BIT;

			if (IsDepthFormat(_description.PixelFormat))
				aspectFlags |= VK_IMAGE_ASPECT_DEPTH_BIT;
		}
		else
		{
			aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
		}

		barrier.subresourceRange.aspectMask = aspectFlags;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = _description.MipCount;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = _description.Layers;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		switch (imageData.CurrentLayout)
		{
		case VK_IMAGE_LAYOUT_UNDEFINED:
		case VK_IMAGE_LAYOUT_GENERAL:
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
		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		{
			barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
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
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
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

	void VulkanImage::Copy(VulkanImage& src, const RectInt& srcRegion, VulkanImageData& dstData, VkImageLayout dstEndingLayout)
	{
		if (src._imageData.CurrentLayout == VK_IMAGE_LAYOUT_UNDEFINED)
		{
			CocoError("Source image has not been initialized")
			return;
		}

		DeviceQueue* queue = _device.GetQueue(DeviceQueue::Type::Graphics);
		if (!queue)
			throw std::exception("A graphics queue is required to transfer pixel data");

		UniqueRef<VulkanCommandBuffer> buffer = queue->Pool.Allocate(true);
		buffer->Begin(true, false);

		VkImageLayout srcLayout = src._imageData.CurrentLayout;
		src.TransitionLayout(*buffer, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		TransitionLayout(*buffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, dstData);

		VkImageCopy copyInfo{};
		copyInfo.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copyInfo.srcSubresource.layerCount = 1;

		copyInfo.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copyInfo.dstSubresource.layerCount = 1;

		copyInfo.srcOffset = { srcRegion.Minimum.X, srcRegion.Minimum.Y, 0 };

		SizeInt srcSize = srcRegion.GetSize();
		copyInfo.extent.width = static_cast<uint32>(srcSize.Width);
		copyInfo.extent.height = static_cast<uint32>(srcSize.Height);
		copyInfo.extent.depth = 1;

		vkCmdCopyImage(buffer->GetCmdBuffer(),
			src._imageData.Image, src._imageData.CurrentLayout,
			dstData.Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &copyInfo);

		src.TransitionLayout(*buffer, srcLayout);
		TransitionLayout(*buffer, dstEndingLayout, dstData);

		buffer->EndAndSubmit();
		_device.WaitForQueueIdle(DeviceQueue::Type::Graphics);
		queue->Pool.Free(*buffer);
	}
	
	void VulkanImage::GenerateMipMaps(const VulkanCommandBuffer& commandBuffer)
	{
		if (_imageData.CurrentLayout != VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
			TransitionLayout(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = _imageData.Image;
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
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
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
				_imageData.Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				_imageData.Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &blit,
				VK_FILTER_LINEAR
			);

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer.GetCmdBuffer(),
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			mipWidth = dstMipWidth;
			mipHeight = dstMipHeight;
		}

		// Transition the last mip level
		barrier.subresourceRange.baseMipLevel = _description.MipCount - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer.GetCmdBuffer(),
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		_imageData.CurrentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	}
}