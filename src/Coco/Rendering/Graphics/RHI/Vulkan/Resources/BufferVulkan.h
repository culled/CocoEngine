#pragma once

#include <Coco/Rendering/Graphics/Resources/Buffer.h>
#include <Coco/Rendering/Graphics/Resources/GraphicsResource.h>

#include "../VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
	class GraphicsDeviceVulkan;

	/// @brief Vulkan-implementation of a Buffer
	class BufferVulkan final : public GraphicsResource<GraphicsDeviceVulkan, Buffer>
	{
	private:
		uint64_t _size;
		VkBuffer _buffer = nullptr;
		VkDeviceMemory _bufferMemory = nullptr;
		VkBufferUsageFlags _usageFlags;
		uint32_t _memoryIndex;
		uint _memoryPropertyFlags;
		bool _isBound = false;
		bool _isLocked = false;

	public:
		BufferVulkan(
			ResourceID id, 
			const string& name, 
			BufferUsageFlags usageFlags, 
			uint64_t size, 
			bool bindOnCreate);
		~BufferVulkan() final;

		DefineResourceType(BufferVulkan)

		void Resize(uint64_t newSize, bool copyOldData) final;
		void CopyTo(uint64_t sourceOffset, Buffer* destination, uint64_t destinationOffset, uint64_t size) final;
		void Bind(uint64_t offset) final;
		uint64_t GetSize() const noexcept final { return _size; }
		void* Lock(uint64_t offset, uint64_t size) final;
		void Unlock() final;

		/// @brief Gets the underlying Vulkan buffer
		/// @return The underlying Vulkan buffer
		VkBuffer GetBuffer() noexcept { return _buffer; }

	private:
		/// @brief Creates a Vulkan buffer
		/// @param size The size of the buffer (in bytes)
		/// @param buffer Will be filled with the created buffer
		/// @param bufferMemory Will be filled with the created buffer's memory
		/// @param bufferMemoryIndex Will be filled with the memory index for the buffer
		void CreateBuffer(uint64_t size, VkBuffer& buffer, VkDeviceMemory& bufferMemory, uint32_t& bufferMemoryIndex);

		/// @brief Destroys a buffer
		/// @param buffer The buffer
		/// @param bufferMemory The buffer memory
		void DestroyBuffer(VkBuffer& buffer, VkDeviceMemory& bufferMemory) noexcept;

		/// @brief Copies data between buffers
		/// @param source The buffer to copy from
		/// @param sourceOffset The start offset to copy from
		/// @param destination The buffer to copy to
		/// @param destinationOffset The start offset to copy to
		/// @param size The number of bytes to copy
		void CopyBuffer(VkBuffer source, uint64_t sourceOffset, VkBuffer destination, uint64_t destinationOffset, uint64_t size);
	};
}