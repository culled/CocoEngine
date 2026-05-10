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

        VulkanShaderBufferInterface* BindOrAllocate(const char* blockName, uint64 instanceID, Ref<VulkanShaderProgram> shaderProgram, VkCommandBuffer commandBuffer);
        void BindDrawTextures(Span<const SharedPtr<Texture>> textures, Ref<VulkanShaderProgram> shaderProgram, VkCommandBuffer commandBuffer);
        void Bind(const char* blockName, uint64 instanceID, VulkanShaderProgram& shaderProgram, VkCommandBuffer commandBuffer);
        bool Has(uint64 id) const;
        void Clear();

    private:
        VulkanGraphicsPlatform* _platform;
        PagedLinearBuffer<VulkanBuffer> _pagedBuffers;
        Map<uint64, VulkanShaderBufferInterface> _interfaces;
        Map<uint64, VulkanDescriptorSetPool> _descriptorSetPools;

        static uint64 GetInterfaceID(const char* blockName, uint64 instanceID, VulkanShaderProgram& shaderProgram);
    };
}

#endif //COCOENGINE_VULKANUNIFORMSTORAGE_H