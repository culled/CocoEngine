//
// Created by cullen on 3/26/26.
//

#ifndef COCOENGINE_VULKANSHADERBUFFERINTERFACE_H
#define COCOENGINE_VULKANSHADERBUFFERINTERFACE_H
#include "Coco/Rendering/Graphics/ShaderBufferInterface.h"
#include "Coco/Core/Memory/Refs.h"
#include "VulkanForwardDeclarations.h"

namespace Coco
{
    struct VulkanPipelineLayout;
    class VulkanGraphicsPlatform;
    class VulkanBuffer;

    struct VulkanDescriptorSetInfo
    {
        VkDescriptorSet DescriptorSet;
        uint64 DescriptorSetIndex;
        Ref<VulkanBuffer> UniformBuffer;
        uint64 BufferOffset;
    };

    class VulkanShaderBufferInterface : public ShaderBufferInterface
    {
    public:
        VulkanShaderBufferInterface(VulkanGraphicsPlatform* platform, slang::TypeLayoutReflection* blockTypeLayout,
            const VulkanDescriptorSetInfo& descriptorSetInfo, const VulkanPipelineLayout* pipelineLayout, VkCommandBuffer commandBuffer);

        void Write(const ShaderElementLocation& location, const void* data, uint64 dataSize) override;
        void Write(const ShaderElementLocation& location, Texture* texture) override;
        void Flush() override;
        void Bind(VkCommandBuffer buffer);

    private:
        VulkanGraphicsPlatform* _platform;
        VulkanDescriptorSetInfo _setInfo;
        const VulkanPipelineLayout* _pipelineLayout;
        VkCommandBuffer _commandBuffer;

    private:
        void WriteUniformBuffer();
    };
} // Coco

#endif //COCOENGINE_VULKANSHADERBUFFERINTERFACE_H