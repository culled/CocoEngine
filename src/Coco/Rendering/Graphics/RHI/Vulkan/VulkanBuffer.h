#pragma once

#include "../../Buffer.h"
#include "../../GraphicsDeviceResource.h"
#include "VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
    class VulkanGraphicsDevice;

    /// @brief Vulkan-implementation of a Buffer
    class VulkanBuffer :
        public Buffer,
        public GraphicsDeviceResource<VulkanGraphicsDevice>
    {
    private:
        BufferUsageFlags _usageFlags;
        Freelist _freelist;
        VkBuffer _buffer;
        VkDeviceMemory _bufferMemory;
        uint32 _bufferMemoryIndex;
        VkMemoryPropertyFlags _memoryProperties;
        bool _isLocked;

    public:
        VulkanBuffer(const GraphicsDeviceResourceID& id, uint64 size, BufferUsageFlags usageFlags, bool bind);
        ~VulkanBuffer();

        // Inherited via Buffer
        BufferUsageFlags GetUsageFlags() const final { return _usageFlags; }
        uint64 GetSize() const final { return _freelist.GetSize(); }
        uint64 GetFreeSpace() const final { return _freelist.GetFreeSpace(); }
        void Resize(uint64 newSize, bool copyOldData) final;
        void CopyTo(uint64 sourceOffset, Buffer& destination, uint64 destinationOffset, uint64 size) final;
        bool Allocate(uint64 requiredSize, FreelistNode& outBlock) final { return _freelist.Allocate(requiredSize, outBlock); }
        void Free(const FreelistNode& block) final { _freelist.Free(block); }
        void* Lock(uint64 offset, uint64 size) final;
        void Unlock() final;

        /// @brief Gets the Vulkan buffer
        /// @return The Vulkan buffer
        VkBuffer GetBuffer() const { return _buffer; }

    private:
        /// @brief Creates a buffer
        /// @param size The size of the buffer
        /// @param outBuffer Will be set to the buffer
        /// @param outBufferMemory Will be set to the buffer's memory
        /// @param outBufferMemoryIndex Will be set to the heap index of the buffer's memory
        void CreateBuffer(uint64 size, VkBuffer& outBuffer, VkDeviceMemory& outBufferMemory, uint32& outBufferMemoryIndex);

        /// @brief Destroys a buffer
        /// @param buffer The buffer
        /// @param bufferMemory The buffer's memory
        void DestroyBuffer(const VkBuffer& buffer, const VkDeviceMemory& bufferMemory);

        /// @brief Copies data from a source to a destination buffer
        /// @param source The source buffer
        /// @param sourceOffset The offset in the source buffer to start copying from, in bytes
        /// @param destination The destination buffer
        /// @param destinationOffset The offset in the destintation buffer to start copying to, in bytes
        /// @param size The number of bytes to copy
        void CopyBuffer(const VkBuffer& source, uint64 sourceOffset, VkBuffer& destination, uint64 destinationOffset, uint64 size);

        /// @brief Binds a buffer to memory
        /// @param buffer The buffer
        /// @param bufferMemory The buffer's memory
        void BindBuffer(VkBuffer buffer, VkDeviceMemory bufferMemory);
    };
}