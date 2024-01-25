#include "Renderpch.h"
#include "VulkanBuffer.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanUtils.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	VulkanBufferInfo::VulkanBufferInfo() :
		VirtualBlock(nullptr),
		Buffer(nullptr),
		Memory(nullptr),
		AllocInfo()
	{}

	VulkanBufferAllocationBlock::VulkanBufferAllocationBlock() :
		VulkanBufferAllocationBlock(0, 0, nullptr)
	{}

	VulkanBufferAllocationBlock::VulkanBufferAllocationBlock(uint64 offset, uint64 size, VmaVirtualAllocation virtualAlloc) :
		Offset(offset),
		Size(size),
		VirtualAlloc(virtualAlloc)
	{}

	bool VulkanBufferAllocationBlock::operator==(const VulkanBufferAllocationBlock& other) const
	{
		return VirtualAlloc == other.VirtualAlloc;
	}

	VulkanBuffer::VulkanBuffer(const GraphicsResourceID& id, VulkanGraphicsDevice& device, uint64 size, BufferUsageFlags usageFlags) :
		Buffer(id),
		_device(device),
		_bufferInfo(),
		_usageFlags(usageFlags),
		_virtualAllocations()
	{
		CreateBuffer(_device, usageFlags, size, _bufferInfo);
	}

	VulkanBuffer::~VulkanBuffer()
	{
		DestroyBuffer(_device, _bufferInfo);
	}

	void VulkanBuffer::LoadData(uint64 offset, const void* data, uint64 dataSize)
	{
		CocoAssert(IsHostVisible(), "Buffer is not host visible. Data must be transferred using a host-visible staging buffer")

		char* mappedMemory = (char*)GetMappedData();
		uint64 size = GetSize() - offset;
		Assert(memcpy_s(mappedMemory + offset, size, data, dataSize) == 0)
	}

	void* VulkanBuffer::GetMappedData() const
	{
		CocoAssert(IsHostVisible(), "Buffer is not host visible. Data must be transferred using a host-visible staging buffer")

		return _bufferInfo.AllocInfo.pMappedData;
	}

	void VulkanBuffer::CopyTo(uint64 sourceOffset, Buffer& destination, uint64 destinationOffset, uint64 size) const
	{
		// TODO: use transfer queue
		VulkanQueue* queue = _device.GetQueue(VulkanQueueType::Graphics);
		if (!queue)
			throw VulkanException("Device does not support transfer operations");

		// Start a command buffer for copying the data
		Ref<VulkanCommandBufferPool> pool = queue->GetCommandPool();
		VulkanCommandBuffer cmdBuffer = pool->Allocate(true);
		cmdBuffer.Begin(true, false);

		VulkanBuffer& vulkanDest = static_cast<VulkanBuffer&>(destination);
		CopyTo(cmdBuffer, sourceOffset, vulkanDest, destinationOffset, size);

		cmdBuffer.EndAndSubmit();

		// Wait for the operation to complete before freeing the command buffer
		queue->WaitForIdle();
		pool->Free(cmdBuffer);
	}

	bool VulkanBuffer::VirtualAllocate(uint64 requiredSize, VulkanBufferAllocationBlock& outBlock)
	{
		VmaVirtualAllocationCreateInfo allocCreateInfo{};
		allocCreateInfo.size = requiredSize;

		VmaVirtualAllocation alloc{};
		VkDeviceSize offset;
		VkResult result = vmaVirtualAllocate(_bufferInfo.VirtualBlock, &allocCreateInfo, &alloc, &offset);

		if (result != VK_SUCCESS)
			return false;

		outBlock = _virtualAllocations.emplace_back(offset, requiredSize, alloc);
		return true;
	}

	void VulkanBuffer::VirtualFree(const VulkanBufferAllocationBlock& block)
	{
		auto it = std::find(_virtualAllocations.begin(), _virtualAllocations.end(), block);

		CocoAssert(it != _virtualAllocations.end(), "Allocated block was not allocated from this buffer")

		vmaVirtualFree(_bufferInfo.VirtualBlock, it->VirtualAlloc);
		_virtualAllocations.erase(it);
	}

	void VulkanBuffer::VirtualFreeAll()
	{
		vmaClearVirtualBlock(_bufferInfo.VirtualBlock);
		_virtualAllocations.clear();
	}

	uint64 VulkanBuffer::GetVirtualFreeSpace() const
	{
		VmaStatistics stats;
		vmaGetVirtualBlockStatistics(_bufferInfo.VirtualBlock, &stats);
		return _bufferInfo.AllocInfo.size - stats.blockBytes;
	}

	void VulkanBuffer::CopyTo(VulkanCommandBuffer& cmdBuffer, uint64 sourceOffset, VulkanBuffer& destination, uint64 destinationOffset, uint64 size) const
	{
		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = sourceOffset;
		copyRegion.dstOffset = destinationOffset;
		copyRegion.size = size;

		vkCmdCopyBuffer(cmdBuffer.GetCmdBuffer(), _bufferInfo.Buffer, destination._bufferInfo.Buffer, 1, &copyRegion);

		VkMemoryBarrier barrier{ VK_STRUCTURE_TYPE_MEMORY_BARRIER };
		barrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

		// TODO: support transfer->graphics barriers
		// TODO: make this based on the type of memory being copied?
		vkCmdPipelineBarrier(cmdBuffer.GetCmdBuffer(),
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
			0,
			1, &barrier,
			0, nullptr,
			0, nullptr);
	}

	void VulkanBuffer::CreateBuffer(VulkanGraphicsDevice& device, BufferUsageFlags usageFlags, uint64 size, VulkanBufferInfo& outBuffer)
	{
		VkBufferCreateInfo createInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		createInfo.size = size;
		createInfo.usage = ToVkBufferUsageFlags(usageFlags);
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocCreateInfo{};
		allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
		allocCreateInfo.flags = 0;

		if ((usageFlags & BufferUsageFlags::HostVisible) == BufferUsageFlags::HostVisible)
		{
			allocCreateInfo.flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
		}

		VmaVirtualBlockCreateInfo virtualCreateInfo{};
		virtualCreateInfo.size = size;

		AssertVkSuccess(vmaCreateVirtualBlock(&virtualCreateInfo, &outBuffer.VirtualBlock))

			AssertVkSuccess(
				vmaCreateBuffer(
					device.GetAllocator(),
					&createInfo,
					&allocCreateInfo,
					&outBuffer.Buffer,
					&outBuffer.Memory,
					&outBuffer.AllocInfo
				)
			)
	}

	void VulkanBuffer::DestroyBuffer(VulkanGraphicsDevice& device, VulkanBufferInfo& buffer)
	{
		if (buffer.VirtualBlock)
		{
			vmaClearVirtualBlock(buffer.VirtualBlock);
			vmaDestroyVirtualBlock(buffer.VirtualBlock);
			buffer.VirtualBlock = nullptr;
		}

		if (buffer.Memory)
		{
			device.WaitForIdle();
			vmaDestroyBuffer(device.GetAllocator(), buffer.Buffer, buffer.Memory);
			buffer.Buffer = nullptr;
			buffer.Memory = nullptr;
		}
	}
}