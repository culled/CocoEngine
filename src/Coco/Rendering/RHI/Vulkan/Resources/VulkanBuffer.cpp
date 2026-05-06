//
// Created by cullen on 3/22/26.
//

#include "Coco/Core/Engine.h"
#include "Coco/Rendering/RHI/Vulkan/VulkanGraphicsPlatform.h"
#include "../VulkanUtils.h"
#include "../VulkanStagingBuffer.h"
#include "VulkanBuffer.h"

namespace Coco
{
    VulkanBufferInfo::VulkanBufferInfo() :
        Buffer(nullptr),
        Memory(nullptr),
        AllocInfo()
    {}

    VulkanBuffer::VulkanBuffer(uint64 id, VulkanGraphicsPlatform* platform, const BufferDescription& description) :
        Buffer(id),
        _platform(platform),
        _description(description)
    {
        CreateBuffer(description, _bufferInfo);

        COCO_ENGINE_LOG_VERBOSE("Created VulkanBuffer %u", id);
    }

    VulkanBuffer::~VulkanBuffer()
    {
        DestroyBuffer(_bufferInfo);

        COCO_ENGINE_LOG_VERBOSE("Destroyed VulkanBuffer %u", GetID());
    }

    void VulkanBuffer::SetData(uint64 offset, Span<const uint8> data)
    {
        // TODO
    }

    void* VulkanBuffer::GetMappedPtr()
    {
        return _bufferInfo.AllocInfo.pMappedData;
    }

    void VulkanBuffer::Resize(uint64 newSize)
    {
        // TODO
    }

    void VulkanBuffer::CopyFrom(StagingOperation& stagingOperation)
    {
        VulkanStagingOperation& vkStagingOperation = static_cast<VulkanStagingOperation&>(stagingOperation);

        VkBufferCopy copyRegion = {
            .srcOffset = vkStagingOperation.BufferOffset,
            .dstOffset = 0,
            .size = vkStagingOperation.Size
        };

        Ref<VulkanBuffer> stagingBuffer = vkStagingOperation.StagingBuffer.Downcast<VulkanBuffer>();
        vkCmdCopyBuffer(
            vkStagingOperation.CommandBuffer,
            stagingBuffer->GetBuffer(),
            _bufferInfo.Buffer,
            1, &copyRegion);

        VulkanStagingBuffer* vkStagingBuffer = static_cast<VulkanStagingBuffer*>(_platform->GetStagingBuffer());
        vkStagingBuffer->AddBufferMemoryBarrier(_bufferInfo.Buffer, 0, stagingOperation.Size);
    }

    void VulkanBuffer::CreateBuffer(const BufferDescription& description, VulkanBufferInfo& outBufferInfo)
    {
        VkBufferCreateInfo createInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        createInfo.size = description.Size;
        createInfo.usage = VulkanUtils::ToVkBufferUsageFlags(description.UsageFlags);
        createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocCreateInfo{};
        allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocCreateInfo.flags = 0;

        if ((description.UsageFlags & BufferUsageFlags::HostVisible) == BufferUsageFlags::HostVisible)
            allocCreateInfo.flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

        AssertVkSuccess(
            vmaCreateBuffer(
                _platform->GetVmaAllocator(),
                &createInfo,
                &allocCreateInfo,
                &outBufferInfo.Buffer,
                &outBufferInfo.Memory,
                &outBufferInfo.AllocInfo
            )
        );
    }

    void VulkanBuffer::DestroyBuffer(VulkanBufferInfo& bufferInfo) noexcept
    {
        if (!bufferInfo.Buffer || !bufferInfo.Memory)
            return;

        _platform->WaitForIdle();

        vmaDestroyBuffer(_platform->GetVmaAllocator(), bufferInfo.Buffer, bufferInfo.Memory);
        bufferInfo.Buffer = nullptr;
        bufferInfo.Memory = nullptr;
        bufferInfo.AllocInfo = {};
    }
} // Coco