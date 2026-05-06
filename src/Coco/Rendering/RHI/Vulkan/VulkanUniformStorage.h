//
// Created by cullen on 3/25/26.
//

#ifndef COCOENGINE_VULKANUNIFORMSTORAGE_H
#define COCOENGINE_VULKANUNIFORMSTORAGE_H


#include "Coco/Core/Types/CoreTypes.h"
#include "Coco/Rendering/Graphics/PagedLinearBuffer.h"
#include "VulkanShaderBufferInterface.h"
#include "VulkanDescriptorSetPool.h"
#include "Resources/VulkanBuffer.h"

namespace Coco
{
    class VulkanShaderProgram;
    class VulkanGraphicsPlatform;

    class VulkanUniformStorage
    {
    public:
        VulkanUniformStorage(VulkanGraphicsPlatform* platform, uint64 pageSize);

        VulkanShaderBufferInterface* Allocate(uint64 id, Ref<VulkanShaderProgram> shaderProgram, const char* blockName, VkCommandBuffer commandBuffer);
        void BindDrawTextures(Span<const SharedPtr<Texture>> textures, Ref<VulkanShaderProgram> shaderProgram, VkCommandBuffer commandBuffer);
        void Bind(uint64 id, VkCommandBuffer commandBuffer);
        bool Has(uint64 id) const;
        void Clear();

    private:
        VulkanGraphicsPlatform* _platform;
        PagedLinearBuffer<VulkanBuffer> _pagedBuffers;
        Map<uint64, VulkanShaderBufferInterface> _interfaces;
        Map<uint64, VulkanDescriptorSetPool> _descriptorSetPools;
    };
}

#endif //COCOENGINE_VULKANUNIFORMSTORAGE_H