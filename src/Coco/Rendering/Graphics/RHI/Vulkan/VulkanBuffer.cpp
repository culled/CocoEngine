#include "Renderpch.h"
#include "VulkanBuffer.h"

#include "VulkanCommandBufferPool.h"
#include "VulkanCommandBuffer.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanUtils.h"

namespace Coco::Rendering::Vulkan
{
	VulkanBuffer::VulkanBuffer(const GraphicsDeviceResourceID& id, uint64 size, BufferUsageFlags usageFlags, bool bind) :
		GraphicsDeviceResource<VulkanGraphicsDevice>(id),
		_buffer(nullptr),
		_usageFlags(usageFlags),
		_freelist(size),
		_lockedMemory(nullptr)
	{
		VkMemoryPropertyFlags localHostMemory = _device.GetFeatures().SupportsHostVisibleLocalMemory ? VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT : 0;

		if ((usageFlags & BufferUsageFlags::HostVisible) == BufferUsageFlags::HostVisible)
			_memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | localHostMemory;
		else
			_memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		CreateBuffer(size, _buffer, _bufferMemory, _bufferMemoryIndex);
		BindBuffer(_buffer, _bufferMemory);
	}

	VulkanBuffer::~VulkanBuffer()
	{
		if (_buffer)
		{
			DestroyBuffer(_buffer, _bufferMemory);
			_buffer = nullptr;
			_bufferMemory = nullptr;
		}
	}

	void VulkanBuffer::Resize(uint64 newSize, bool copyOldData)
	{
		if (_lockedMemory)
			Unlock();

		_freelist.Resize(newSize);

		// Create a new buffer at the requested size
		VkBuffer newBuffer;
		VkDeviceMemory newBufferMemory;
		uint32 newBufferMemoryIndex;
		CreateBuffer(newSize, newBuffer, newBufferMemory, newBufferMemoryIndex);

		BindBuffer(newBuffer, newBufferMemory);

		// Copy data from the current buffer to the resized one
		if (copyOldData)
			CopyBuffer(_buffer, 0, newBuffer, 0, newSize);

		// Destroy our current buffer
		DestroyBuffer(_buffer, _bufferMemory);
		
		// Make the resized buffer our current buffer
		_buffer = newBuffer;
		_bufferMemory = newBufferMemory;
		_bufferMemoryIndex = newBufferMemoryIndex;
	}

	void VulkanBuffer::CopyTo(uint64 sourceOffset, Buffer& destination, uint64 destinationOffset, uint64 size)
	{
		VulkanBuffer* vulkanBuffer = static_cast<VulkanBuffer*>(&destination);
		CopyBuffer(_buffer, sourceOffset, vulkanBuffer->_buffer, destinationOffset, size);
	}

	void* VulkanBuffer::Lock(uint64 offset, uint64 size)
	{
		if(!_lockedMemory)
			AssertVkSuccess(vkMapMemory(_device.GetDevice(), _bufferMemory, offset, size, 0, &_lockedMemory));
		return _lockedMemory;
	}

	void VulkanBuffer::Unlock()
	{
		vkUnmapMemory(_device.GetDevice(), _bufferMemory);
		_lockedMemory = nullptr;
	}

	void VulkanBuffer::CreateBuffer(uint64 size, VkBuffer& outBuffer, VkDeviceMemory& outBufferMemory, uint32& outBufferMemoryIndex)
	{
		VkBufferCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.size = size;
		createInfo.usage = ToVkBufferUsageFlags(_usageFlags);
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		AssertVkSuccess(vkCreateBuffer(_device.GetDevice(), &createInfo, _device.GetAllocationCallbacks(), &outBuffer));

		// Get the memory requirements for the buffer
		VkMemoryRequirements memoryRequirements{};
		vkGetBufferMemoryRequirements(_device.GetDevice(), outBuffer, &memoryRequirements);

		if (!_device.FindMemoryIndex(memoryRequirements.memoryTypeBits, _memoryProperties, outBufferMemoryIndex))
			throw std::exception("Unable to create VulkanBuffer because the required memory type could not be found");

		// Allocate memory for the buffer
		VkMemoryAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocateInfo.allocationSize = memoryRequirements.size;
		allocateInfo.memoryTypeIndex = outBufferMemoryIndex;

		AssertVkSuccess(vkAllocateMemory(_device.GetDevice(), &allocateInfo, _device.GetAllocationCallbacks(), &outBufferMemory));
	}

	void VulkanBuffer::DestroyBuffer(const VkBuffer& buffer, const VkDeviceMemory& bufferMemory)
	{
		_device.WaitForIdle();

		if (bufferMemory)
		{
			vkFreeMemory(_device.GetDevice(), bufferMemory, _device.GetAllocationCallbacks());
		}

		if (buffer)
		{
			vkDestroyBuffer(_device.GetDevice(), buffer, _device.GetAllocationCallbacks());
		}
	}

	void VulkanBuffer::CopyBuffer(const VkBuffer& source, uint64 sourceOffset, VkBuffer& destination, uint64 destinationOffset, uint64 size)
	{
		DeviceQueue* transferQueue = _device.GetQueue(DeviceQueue::Type::Transfer);
		if (!transferQueue)
			throw std::exception("Device does not support transfer operations");

		// Start a command buffer for copying the data
		UniqueRef<VulkanCommandBuffer> cmdBuffer = transferQueue->Pool.Allocate(true);
		cmdBuffer->Begin(true, false);

		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = sourceOffset;
		copyRegion.dstOffset = destinationOffset;
		copyRegion.size = size;

		vkCmdCopyBuffer(cmdBuffer->GetCmdBuffer(), source, destination, 1, &copyRegion);

		cmdBuffer->EndAndSubmit();

		// Wait for the operation to complete before freeing the command buffer
		_device.WaitForQueueIdle(transferQueue->QueueType);
		transferQueue->Pool.Free(*cmdBuffer);
	}

	void VulkanBuffer::BindBuffer(VkBuffer buffer, VkDeviceMemory bufferMemory)
	{
		AssertVkSuccess(vkBindBufferMemory(_device.GetDevice(), buffer, bufferMemory, 0))
	}
}
