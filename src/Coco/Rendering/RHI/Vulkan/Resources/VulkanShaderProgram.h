//
// Created by cullen on 3/22/26.
//

#ifndef COCOENGINE_VULKANSHADERPROGRAM_H
#define COCOENGINE_VULKANSHADERPROGRAM_H

#include "Coco/Rendering/Graphics/Resources/ShaderProgram.h"
#include "../VulkanForwardDeclarations.h"

#include "Coco/Core/Types/StackArray.h"
#include "Coco/Rendering/Graphics/VertexDataTypes.h"

#include "Coco/Core/Types/Array.h"
#include "Coco/Rendering/RHI/Vulkan/VulkanDescriptorSetLayout.h"
#include <slang-com-ptr.h>

#include "Coco/Rendering/RHI/Vulkan/VulkanPipelineLayout.h"

namespace Coco
{
    class VulkanGraphicsPlatform;

    class VulkanShaderProgram : public ShaderProgram
    {
    public:
        VulkanShaderProgram(uint64 id, VulkanGraphicsPlatform* platform, const FilePath& shaderPath);
        ~VulkanShaderProgram();

        slang::ProgramLayout* GetProgramLayout() override;
        int64 GetParamBlockIndex(const char* name) override;
        slang::TypeLayoutReflection* GetParamBlockLayout(uint64 index) override;

        Span<const VertexChannel> GetVertexChannels() const { return _vertexChannels; }
        Span<const VulkanDescriptorSetLayout> GetDescriptorSetLayouts() const { return _pipelineLayout.DescriptorSetLayouts; }
        const VulkanPipelineLayout* GetPipelineLayout() const { return &_pipelineLayout; }
        VkPipelineVertexInputStateCreateInfo GetVertexInputStateCreateInfo() const;
        void GetStageInfos(ArrayContainer<VkPipelineShaderStageCreateInfo>& outStageInfos) const;

    private:
        VulkanGraphicsPlatform* _platform;
        Slang::ComPtr<slang::IComponentType> _linkedProgram;
        slang::TypeLayoutReflection* _globalUniformsLayoutInfo;
        Array<VkVertexInputBindingDescription> _vertexInputBindingDescriptions;
        Array<VkVertexInputAttributeDescription> _vertexInputAttributeDescriptions;
        VulkanPipelineLayout _pipelineLayout;
        VkShaderModule _shaderModule;
        StackArray<VertexChannel, 5> _vertexChannels;

    private:
        void ReflectVertexInputInformation();
        void CreatePipelineLayout();
    };
} // Coco

#endif //COCOENGINE_VULKANSHADERPROGRAM_H