#pragma once

#include "../../Buffer.h"
#include "../../GraphicsDeviceResource.h"
#include "VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
    class VulkanGraphicsDevice;

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

        VkBuffer GetBuffer() const { return _buffer; }

    private:
        void CreateBuffer(uint64 size, VkBuffer& outBuffer, VkDeviceMemory& outBufferMemory, uint32& outBufferMemoryIndex);
        void DestroyBuffer(const VkBuffer& buffer, const VkDeviceMemory& bufferMemory);
        void CopyBuffer(const VkBuffer& source, uint64 sourceOffset, VkBuffer& destination, uint64 destinationOffset, uint64 size);

        void BindBuffer(VkBuffer buffer, VkDeviceMemory bufferMemory);
    };
}