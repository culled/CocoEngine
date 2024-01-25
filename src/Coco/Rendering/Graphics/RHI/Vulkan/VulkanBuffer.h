#pragma once
#include "../../Buffer.h"
#include "../../../Renderpch.h"
#include "VulkanIncludes.h"
#include <vk_mem_alloc.h>

namespace Coco::Rendering::Vulkan
{
    class VulkanGraphicsDevice;
    class VulkanCommandBuffer;

    struct VulkanBufferInfo
    {
        VmaVirtualBlock VirtualBlock;
        VkBuffer Buffer;
        VmaAllocation Memory;
        VmaAllocationInfo AllocInfo;

        VulkanBufferInfo();
    };

    struct VulkanBufferAllocationBlock
    {
        uint64 Offset;
        uint64 Size;
        VmaVirtualAllocation VirtualAlloc;

        VulkanBufferAllocationBlock();
        VulkanBufferAllocationBlock(uint64 offset, uint64 size, VmaVirtualAllocation virtualAlloc);

        bool operator==(const VulkanBufferAllocationBlock& other) const;
    };

    class VulkanBuffer :
        public Buffer
    {
    public:
        VulkanBuffer(const GraphicsResourceID& id, VulkanGraphicsDevice& device, uint64 size, BufferUsageFlags usageFlags);
        ~VulkanBuffer();

        // Inherited via Buffer
        BufferUsageFlags GetUsageFlags() const final { return _usageFlags; }
        bool IsHostVisible() const final { return (_usageFlags & BufferUsageFlags::HostVisible) == BufferUsageFlags::HostVisible; }
        uint64 GetSize() const final { return _bufferInfo.AllocInfo.size; }
        void LoadData(uint64 offset, const void* data, uint64 dataSize) final;
        void* GetMappedData() const final;
        void CopyTo(uint64 sourceOffset, Buffer& destination, uint64 destinationOffset, uint64 size) const final;

        bool VirtualAllocate(uint64 requiredSize, VulkanBufferAllocationBlock& outBlock);
        void VirtualFree(const VulkanBufferAllocationBlock& block);
        void VirtualFreeAll();
        uint64 GetVirtualFreeSpace() const;

        void CopyTo(VulkanCommandBuffer& cmdBuffer, uint64 sourceOffset, VulkanBuffer& destination, uint64 destinationOffset, uint64 size) const;

        VkBuffer GetBuffer() const { return _bufferInfo.Buffer; }

    private:
        VulkanGraphicsDevice& _device;

        VulkanBufferInfo _bufferInfo;
        BufferUsageFlags _usageFlags;
        std::list<VulkanBufferAllocationBlock> _virtualAllocations;

    private:
        static void CreateBuffer(VulkanGraphicsDevice& device, BufferUsageFlags usageFlags, uint64 size, VulkanBufferInfo& outBuffer);
        static void DestroyBuffer(VulkanGraphicsDevice& device, VulkanBufferInfo& buffer);
    };
}