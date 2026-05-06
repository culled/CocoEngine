//
// Created by cullen on 3/22/26.
//

#ifndef COCOENGINE_VULKANBUFFER_H
#define COCOENGINE_VULKANBUFFER_H
#include "Coco/Rendering/Graphics/Resources/Buffer.h"
#include "Coco/Rendering/Graphics/Resources/BufferTypes.h"

#include "Coco/Rendering/RHI/Vulkan/VulkanIncludes.h"
#include <vk_mem_alloc.h>

namespace Coco
{
    class VulkanGraphicsPlatform;

    struct VulkanBufferInfo
    {
        /// @brief The buffer
        VkBuffer Buffer;

        /// @brief The buffer memory
        VmaAllocation Memory;

        /// @brief Allocation information for the buffer
        VmaAllocationInfo AllocInfo;

        VulkanBufferInfo();
    };

    class VulkanBuffer : public Buffer
    {
    public:
        VulkanBuffer(uint64 id, VulkanGraphicsPlatform* platform, const BufferDescription& description);
        ~VulkanBuffer();

        uint64 GetSize() const override { return _description.Size; }
        void SetData(uint64 offset, Span<const uint8> data) override;
        void* GetMappedPtr() override;
        void Resize(uint64 newSize) override;
        void CopyFrom(StagingOperation& stagingOperation) override;

        VkBuffer GetBuffer() const { return _bufferInfo.Buffer; }

    private:
        VulkanGraphicsPlatform* _platform;
        BufferDescription _description;
        VulkanBufferInfo _bufferInfo;

    private:
        void CreateBuffer(const BufferDescription& description, VulkanBufferInfo& outBufferInfo);

        /// @brief Destroys a buffer
        /// @param bufferInfo The buffer to destroy
        void DestroyBuffer(VulkanBufferInfo& bufferInfo) noexcept;
    };
} // Coco

#endif //COCOENGINE_VULKANBUFFER_H