//
// Created by cullen on 3/26/26.
//

#include "Coco/Rendering/Graphics/ShaderCursor.h"
#include "VulkanShaderBufferInterface.h"
#include "VulkanGraphicsPlatform.h"
#include "Coco/Rendering/Texture.h"

#include "Coco/Rendering/RenderService.h"

#include "VulkanUtils.h"

#include "VulkanPipelineLayout.h"

#include "Resources/VulkanBuffer.h"
#include "Resources/VulkanImage.h"
#include "Resources/VulkanImageSampler.h"

#include <slang.h>

namespace Coco
{
    VulkanShaderBufferInterface::VulkanShaderBufferInterface(VulkanGraphicsPlatform* platform, slang::TypeLayoutReflection* blockTypeLayout,
        const VulkanDescriptorSetInfo& descriptorSetInfo, const VulkanPipelineLayout* pipelineLayout, VkCommandBuffer commandBuffer) :
        ShaderBufferInterface(blockTypeLayout),
        _platform(platform),
        _setInfo(descriptorSetInfo),
        _pipelineLayout(pipelineLayout),
        _commandBuffer(commandBuffer)
    {
        if (_blockTypeLayout->getSize() > 0)
            WriteUniformBuffer();
    }

    void VulkanShaderBufferInterface::Write(const ShaderElementLocation& location, const void* data, uint64 dataSize)
    {
        uint8* bufferPtr = static_cast<uint8*>(_setInfo.UniformBuffer->GetMappedPtr()) + _setInfo.BufferOffset;
        memcpy(bufferPtr + location.ByteOffset, data, dataSize);
    }

    void VulkanShaderBufferInterface::Write(const ShaderElementLocation& location, Texture* texture)
    {
        if (!texture)
        {
            RenderService* rendering = _platform->GetRenderService();
            texture = rendering->GetDefaultCheckerTexture().get();
        }

        Ref<VulkanImage> image = texture->GetImage().Downcast<VulkanImage>();
        Ref<VulkanImageSampler> imageSampler = texture->GetSampler().Downcast<VulkanImageSampler>();

        VkDescriptorImageInfo imageInfo;
        imageInfo.imageView = image->GetNativeView();
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.sampler = imageSampler->GetSampler();

        VkWriteDescriptorSet write = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
        write.dstSet = _setInfo.DescriptorSet;
        write.dstBinding = location.BindingRangeOffset;
        write.dstArrayElement = 0;
        write.descriptorCount = 1;
        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(_platform->GetDevice(), 1, &write, 0, nullptr);
    }

    void VulkanShaderBufferInterface::Flush()
    {
        // TODO
    }

    void VulkanShaderBufferInterface::Bind(VkCommandBuffer buffer)
    {
        vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout->PipelineLayout, _setInfo.DescriptorSetIndex, 1, &_setInfo.DescriptorSet, 0, nullptr);
    }

    void VulkanShaderBufferInterface::WriteUniformBuffer()
    {
        VkDescriptorBufferInfo bufferInfo;
        bufferInfo.buffer = _setInfo.UniformBuffer->GetBuffer();
        bufferInfo.offset = _setInfo.BufferOffset;
        bufferInfo.range = _blockTypeLayout->getSize();

        VkWriteDescriptorSet write = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
        write.dstSet = _setInfo.DescriptorSet;
        write.dstBinding = 0;
        write.dstArrayElement = 0;
        write.descriptorCount = 1;
        write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(_platform->GetDevice(), 1, &write, 0, nullptr);
    }
} // Coco