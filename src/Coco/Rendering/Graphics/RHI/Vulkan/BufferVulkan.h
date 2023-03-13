#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Rendering/Graphics/Buffer.h>

#include "VulkanIncludes.h"

namespace Coco::Rendering
{
	class GraphicsDevice;
	class GraphicsDeviceVulkan;

	/// <summary>
	/// Vulkan-implementation of a Buffer
	/// </summary>
	class BufferVulkan : public Buffer
	{
	private:
		GraphicsDeviceVulkan* _device;
		uint64_t _size;
		VkBuffer _buffer = nullptr;
		VkDeviceMemory _bufferMemory = nullptr;
		VkBufferUsageFlags _usageFlags;
		uint32_t _memoryIndex;
		uint _memoryPropertyFlags;
		bool _isBound = false;
		bool _isLocked = false;

	public:
		BufferVulkan(GraphicsDevice* owningDevice, BufferUsageFlags usageFlags, uint64_t size, uint memoryPropertyFlags, bool createBound);
		virtual ~BufferVulkan() override;

		virtual void Resize(uint64_t newSize) override;
		virtual void CopyTo(uint64_t sourceOffset, Buffer* destination, uint64_t destinationOffset, uint64_t size) override;
		virtual void Bind(uint64_t offset) override;
		virtual uint64_t GetSize() const override { return _size; }
		virtual void* Lock(uint64_t offset, uint64_t size) override;
		virtual void Unlock() override;

		/// <summary>
		/// Gets the underlying Vulkan buffer
		/// </summary>
		/// <returns>The underlying Vulkan buffer</returns>
		VkBuffer GetBuffer() const { return _buffer; }

	private:
		/// <summary>
		/// Creates a Vulkan buffer
		/// </summary>
		/// <param name="size">The size of the buffer (in bytes)</param>
		/// <param name="buffer">The buffer</param>
		/// <param name="bufferMemory">The buffer memory</param>
		/// <param name="bufferMemoryIndex">The index of memory used for the buffer</param>
		void CreateBuffer(uint64_t size, VkBuffer* buffer, VkDeviceMemory* bufferMemory, uint32_t& bufferMemoryIndex);

		/// <summary>
		/// Destroys a buffer
		/// </summary>
		/// <param name="buffer">The buffer</param>
		/// <param name="bufferMemory">The buffer's memory</param>
		void DestroyBuffer(VkBuffer buffer, VkDeviceMemory bufferMemory);

		/// <summary>
		/// Copies data between buffers
		/// </summary>
		/// <param name="source">The buffer to copy from</param>
		/// <param name="sourceOffset">The start offset to copy from</param>
		/// <param name="destination">The buffer to copy to</param>
		/// <param name="destinationOffset">The start offset to copy to</param>
		/// <param name="size">The number of bytes to copy</param>
		void CopyBuffer(VkBuffer source, uint64_t sourceOffset, VkBuffer destination, uint64_t destinationOffset, uint64_t size);
	};
}