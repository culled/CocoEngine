//
// Created by cullen on 3/26/26.
//

#ifndef COCOENGINE_VULKANDESCRIPTORSETPOOL_H
#define COCOENGINE_VULKANDESCRIPTORSETPOOL_H
#include "Coco/Core/Memory/Refs.h"

#include "Resources/VulkanShaderProgram.h"
#include "Coco/Rendering/RHI/Vulkan/VulkanIncludes.h"

namespace Coco
{
    struct VulkanDescriptorSetPoolInfo
    {
        /// @brief The pool
        VkDescriptorPool Pool;

        /// @brief The last frame number when this pool was allocated from
        uint64 LastAllocatedFrameNumber;
    };

    class VulkanDescriptorSetPool
    {
    public:
        VulkanDescriptorSetPool(VulkanGraphicsPlatform* platform, Ref<VulkanShaderProgram> shaderProgram);
        ~VulkanDescriptorSetPool();

        VkDescriptorSet AllocateDescriptorSet(uint64 layoutIndex);
        void Reset();
        uint64 GetLastAllocatedFrameNumber() const { return _lastAllocatedFrameNumber; }

    private:
        static constexpr uint32 _maxSets = 512;
        static constexpr uint64 _maxDescriptors = 65536;

        VulkanGraphicsPlatform* _platform;
        Ref<VulkanShaderProgram> _shaderProgram;
        Array<VulkanDescriptorSetPoolInfo> _pools;
        Array<VkDescriptorPoolSize> _poolSizes;
        uint64 _lastAllocatedFrameNumber;
        VkDescriptorPoolCreateInfo _poolCreateInfo;
    };
} // Coco

#endif //COCOENGINE_VULKANDESCRIPTORSETPOOL_H