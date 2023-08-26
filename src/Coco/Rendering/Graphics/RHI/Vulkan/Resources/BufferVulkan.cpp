#include "BufferVulkan.h"

#include "../GraphicsDeviceVulkan.h"
#include "../VulkanUtilities.h"
#include "../CommandBufferPoolVulkan.h"
#include "CommandBufferVulkan.h"

namespace Coco::Rendering::Vulkan
{
	BufferVulkan::BufferVulkan(const ResourceID& id,
		const string& name,
		BufferUsageFlags usageFlags,
		uint64_t size,
		bool bindOnCreate) :
		GraphicsResource<GraphicsDeviceVulkan, Buffer>(id, name, usageFlags),
		_size(size), _freelist(size)
	{
		// TODO: configurable memory properties?
		uint localHostMemory = _device->GetMemoryFeatures().SupportsLocalHostBufferMemory ? VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT : 0;

		if ((usageFlags & BufferUsageFlags::HostVisible) == BufferUsageFlags::HostVisible)
			_memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | localHostMemory;
		else
			_memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		_usageFlags = ToVkBufferUsageFlags(usageFlags);

		CreateBuffer(size, _buffer, _bufferMemory, _memoryIndex);

		if (bindOnCreate)
			Bind(0);
	}

	BufferVulkan::~BufferVulkan()
	{
		_device->WaitForIdle();

		DestroyBuffer(_buffer, _bufferMemory);

		_size = 0;
	}

	void BufferVulkan::Resize(uint64_t newSize, bool copyOldData)
	{
		_freelist.Resize(newSize);

		// Create a new buffer at the requested size
		VkBuffer newBuffer;
		VkDeviceMemory newBufferMemory;
		uint32_t newBufferMemoryIndex;
		CreateBuffer(newSize, newBuffer, newBufferMemory, newBufferMemoryIndex);

		// Bind the new buffer
		AssertVkResult(vkBindBufferMemory(_device->GetDevice(), newBuffer, newBufferMemory, 0));

		// Copy data from the current buffer to the resized one
		if(copyOldData)
			CopyBuffer(_buffer, 0, newBuffer, 0, newSize);

		// Destroy our current buffer
		DestroyBuffer(_buffer, _bufferMemory);

		// Make the resized buffer our current buffer
		_buffer = newBuffer;
		_bufferMemory = newBufferMemory;
		_memoryIndex = newBufferMemoryIndex;

		// The buffer is bound due to the copy
		_isBound = true;
	}

	void BufferVulkan::CopyTo(uint64_t sourceOffset, Buffer* destination, uint64_t destinationOffset, uint64_t size)
	{
		BufferVulkan* dest = static_cast<BufferVulkan*>(destination);
		CopyBuffer(_buffer, sourceOffset, dest->_buffer, destinationOffset, size);
	}

	void BufferVulkan::Bind(uint64_t offset)
	{
		if (_isBound)
			return;

		AssertVkResult(vkBindBufferMemory(_device->GetDevice(), _buffer, _bufferMemory, offset));
		_isBound = true;
	}

	bool BufferVulkan::Allocate(uint64_t requiredSize, FreelistAllocatedBlock& block)
	{
		return _freelist.Allocate(requiredSize, block);
	}

	void BufferVulkan::Free(const FreelistAllocatedBlock& block)
	{
		_freelist.Return(block);
	}

	void* BufferVulkan::Lock(uint64_t offset, uint64_t size)
	{
		void* data;
		AssertVkResult(vkMapMemory(_device->GetDevice(), _bufferMemory, offset, size, 0, &data));
		_isLocked = true;
		return data;
	}

	void BufferVulkan::Unlock()
	{
		vkUnmapMemory(_device->GetDevice(), _bufferMemory);
		_isLocked = false;
	}

	void BufferVulkan::CreateBuffer(uint64_t size, VkBuffer& buffer, VkDeviceMemory& bufferMemory, uint32_t& bufferMemoryIndex)
	{
		VkBufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.size = size;
		createInfo.usage = _usageFlags;
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		AssertVkResult(vkCreateBuffer(_device->GetDevice(), &createInfo, nullptr, &buffer));

		// Get the memory requirements for the buffer
		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(_device->GetDevice(), buffer, &memoryRequirements);

		if (!_device->FindMemoryIndex(memoryRequirements.memoryTypeBits, _memoryPropertyFlags, bufferMemoryIndex))
			throw VulkanRenderingException("Unable to create Vulkan buffer because the required memory type could not be found");

		// Allocate memory for the buffer
		VkMemoryAllocateInfo allocateInfo = {};
		allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocateInfo.allocationSize = memoryRequirements.size;
		allocateInfo.memoryTypeIndex = static_cast<uint32_t>(bufferMemoryIndex);

		AssertVkResult(vkAllocateMemory(_device->GetDevice(), &allocateInfo, nullptr, &bufferMemory));
	}

	void BufferVulkan::DestroyBuffer(VkBuffer& buffer, VkDeviceMemory& bufferMemory) noexcept
	{
		// Wait for operations potentially using the buffer to finish before destroying it
		_device->WaitForIdle();

		if (bufferMemory != nullptr)
		{
			vkFreeMemory(_device->GetDevice(), bufferMemory, nullptr);
		}

		bufferMemory = nullptr;

		if (buffer != nullptr)
		{
			vkDestroyBuffer(_device->GetDevice(), buffer, nullptr);
		}

		buffer = nullptr;
	}

	void BufferVulkan::CopyBuffer(VkBuffer source, uint64_t sourceOffset, VkBuffer destination, uint64_t destinationOffset, uint64_t size)
	{
		Ref<CommandBufferPoolVulkan> pool;

		if (!_device->GetTransferCommandPool(pool))
			throw BufferTransferException("Cannot copy buffer without a transfer queue");

		// Start a command buffer for copying the data
		Ref<CommandBufferVulkan> buffer = pool->Allocate(true);
		buffer->Begin(true, false);

		VkBufferCopy copyRegion = {};
		copyRegion.srcOffset = sourceOffset;
		copyRegion.dstOffset = destinationOffset;
		copyRegion.size = size;

		vkCmdCopyBuffer(buffer->GetCmdBuffer(), source, destination, 1, &copyRegion);

		buffer->EndAndSubmit();

		// Wait for the operation to complete and free the buffer
		pool->WaitForQueue();
		pool->Free(buffer);
	}
}
