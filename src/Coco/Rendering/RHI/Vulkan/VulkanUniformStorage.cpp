//
// Created by cullen on 3/25/26.
//

#include "VulkanUniformStorage.h"
#include "VulkanGraphicsPlatform.h"

#include "Coco/Core/Engine.h"
#include "Coco/Rendering/RenderService.h"
#include "Coco/Rendering/Texture.h"

#include "Resources/VulkanBuffer.h"
#include "Resources/VulkanImage.h"
#include "Resources/VulkanImageSampler.h"
#include "Resources/VulkanShaderProgram.h"

namespace Coco
{
    VulkanUniformStorage::VulkanUniformStorage(VulkanGraphicsPlatform* platform, uint64 pageSize) :
        _platform(platform),
        _pagedBuffers(platform, BufferDescription(pageSize, BufferUsageFlags::HostVisible | BufferUsageFlags::Uniform), _platform->GetDeviceDescription().MinimumBufferAlignment),
        _interfaces()
    {}

    VulkanShaderBufferInterface* VulkanUniformStorage::Allocate(uint64 id, Ref<VulkanShaderProgram> shaderProgram,
        const char* blockName, VkCommandBuffer commandBuffer)
    {
        if (_interfaces.Contains(id))
            return &_interfaces.Get(id);

        auto programLayout = shaderProgram->GetProgramLayout();
        auto globalTypeLayout = programLayout->getGlobalParamsTypeLayout();
        int64 blockIndex = globalTypeLayout->findFieldIndexByName(blockName);
        if (blockIndex == -1)
        {
            COCO_ENGINE_LOG_ERROR("Invalid uniform block \"%s\"", blockName);
            return nullptr;
        }

        const VulkanPipelineLayout* pipelineLayout = shaderProgram->GetPipelineLayout();
        auto descriptorSetLayouts = shaderProgram->GetDescriptorSetLayouts();

        VulkanDescriptorSetInfo setInfo;
        setInfo.DescriptorSetIndex = pipelineLayout->GlobalDescriptorSetIndexOffset + blockIndex;
        auto& descriptorSetLayout = descriptorSetLayouts[setInfo.DescriptorSetIndex];
        COCO_ASSERT(!descriptorSetLayout.LayoutBindings.IsEmpty(), "Uniform block contained no bindings");

        slang::TypeLayoutReflection* blockLayout = nullptr;

        auto field = globalTypeLayout->getFieldByIndex(blockIndex);
        blockLayout = field->getTypeLayout()->getElementTypeLayout();

        uint64 dataSize = blockLayout->getSize();
        if (dataSize > 0)
            _pagedBuffers.Allocate(dataSize, setInfo.UniformBuffer, setInfo.BufferOffset);

        auto pool = _descriptorSetPools.TryGetValue(shaderProgram->GetID());
        if (!pool)
            pool = &_descriptorSetPools.Emplace(shaderProgram->GetID(), _platform, shaderProgram);

        setInfo.DescriptorSet = pool->AllocateDescriptorSet(setInfo.DescriptorSetIndex);

        auto& interface = _interfaces.Emplace(id, _platform, blockLayout, setInfo, pipelineLayout, commandBuffer);
        return &interface;
    }

    void VulkanUniformStorage::BindDrawTextures(Span<const SharedPtr<Texture>> textures,
                                                Ref<VulkanShaderProgram> shaderProgram, VkCommandBuffer commandBuffer)
    {
        auto descriptorSetLayouts = shaderProgram->GetDescriptorSetLayouts();

        auto pool = _descriptorSetPools.TryGetValue(shaderProgram->GetID());
        if (!pool)
            pool = &_descriptorSetPools.Emplace(shaderProgram->GetID(), _platform, shaderProgram);

        StackArray<VkDescriptorSet, 2> descriptorSets;
        StackArray<VkWriteDescriptorSet, 32> writes;
        StackArray<VkDescriptorImageInfo, 32> imageInfos;
        RenderService* rendering = _platform->GetRenderService();
        uint64 currentTextureIndex = 0;
        uint32 firstSetIndex = std::numeric_limits<uint32>::max();

        for (const auto& descriptorSetLayout : descriptorSetLayouts)
        {
            if (descriptorSetLayout.ShaderStages == ShaderStageFlags::All)
                continue;

            if (firstSetIndex == std::numeric_limits<uint32>::max())
                firstSetIndex = descriptorSetLayout.DescriptorSetIndex;

            VkDescriptorSet descriptorSet = pool->AllocateDescriptorSet(descriptorSetLayout.DescriptorSetIndex);
            descriptorSets.Append(descriptorSet);

            for (const auto& setBindings : descriptorSetLayout.LayoutBindings)
            {
                Texture* tex;
                if (currentTextureIndex >= textures.size() || textures[currentTextureIndex] == nullptr)
                {
                    tex = rendering->GetDefaultCheckerTexture().get();
                }
                else
                {
                    tex = textures[currentTextureIndex].get();
                }

                Ref<VulkanImage> image = tex->GetImage().Downcast<VulkanImage>();
                Ref<VulkanImageSampler> imageSampler = tex->GetSampler().Downcast<VulkanImageSampler>();

                VkDescriptorImageInfo& imageInfo = imageInfos.EmplaceBack();
                imageInfo.imageView = image->GetNativeView();
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.sampler = imageSampler->GetSampler();

                VkWriteDescriptorSet& write = writes.EmplaceBack(VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET);
                write.dstSet = descriptorSet;
                write.dstBinding = setBindings.binding;
                write.dstArrayElement = 0;
                write.descriptorCount = 1;
                write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                write.pImageInfo = &imageInfo;

                currentTextureIndex++;
            }
        }

        vkUpdateDescriptorSets(_platform->GetDevice(), static_cast<uint32>(writes.GetCount()), writes.Data(), 0, nullptr);

        const VulkanPipelineLayout* pipelineLayout = shaderProgram->GetPipelineLayout();
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout->PipelineLayout,
            firstSetIndex, static_cast<uint32>(descriptorSets.GetCount()), descriptorSets.Data(),
            0, nullptr);
    }

    void VulkanUniformStorage::Bind(uint64 id, VkCommandBuffer commandBuffer)
    {
        if (auto interface = _interfaces.TryGetValue(id))
        {
            interface->Bind(commandBuffer);
        }
    }

    bool VulkanUniformStorage::Has(uint64 id) const
    {
        return _interfaces.Contains(id);
    }

    void VulkanUniformStorage::Clear()
    {
        _pagedBuffers.Clear();
        _interfaces.Clear();

        for (auto& pool : _descriptorSetPools)
        {
            pool.second.Reset();

            // TODO: free stale pools
            //if (_platform->GetCurrentFrameNumber() - pool.second.GetLastAllocatedFrameNumber())
            //{

            //}
        }
    }
}
