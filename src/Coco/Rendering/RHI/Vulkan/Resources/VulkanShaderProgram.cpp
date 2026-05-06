//
// Created by cullen on 3/22/26.
//

#include "VulkanShaderProgram.h"

#include "../VulkanGraphicsPlatform.h"

#include "Coco/Core/Engine.h"
#include "Coco/Rendering/Graphics/Slang/SlangCompiler.h"
#include "Coco/Rendering/RHI/Vulkan/VulkanIncludes.h"
#include "Coco/Rendering/RHI/Vulkan/VulkanPipelineLayoutBuilder.h"

#include "Coco/Rendering/RHI/Vulkan/VulkanUtils.h"
#include "../VulkanIncludes.h"

namespace Coco
{
    VulkanShaderProgram::VulkanShaderProgram(uint64 id, VulkanGraphicsPlatform* platform, const FilePath& shaderPath) :
        ShaderProgram(id),
        _platform(platform),
        _linkedProgram(),
        _globalUniformsLayoutInfo(nullptr),
        _pipelineLayout(),
        _shaderModule(nullptr),
        _vertexChannels()
    {
        _linkedProgram = _platform->GetShaderProgramCompiler()->CompileShader(shaderPath);

        Slang::ComPtr<slang::IBlob> programCode;
        auto result = _linkedProgram->getTargetCode(0, programCode.writeRef());
        COCO_ASSERT(SLANG_SUCCEEDED(result), "Failed to get program code");

        auto layout = _linkedProgram->getLayout();
        _globalUniformsLayoutInfo = layout->getGlobalParamsVarLayout()->getTypeLayout();

        VkShaderModuleCreateInfo createInfo{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
        createInfo.flags = 0;
        createInfo.codeSize = programCode->getBufferSize();
        createInfo.pCode = static_cast<const uint32_t*>(programCode->getBufferPointer());
        AssertVkSuccess(vkCreateShaderModule(_platform->GetDevice(), &createInfo, _platform->GetAllocationCallbacks(), &_shaderModule));

        ReflectVertexInputInformation();
        CreatePipelineLayout();

        COCO_ENGINE_LOG_VERBOSE("Created VulkanShaderProgram %u for module \"%s\"", id, shaderPath.CStr());
    }

    VulkanShaderProgram::~VulkanShaderProgram()
    {
        _platform->WaitForIdle();

        if (_pipelineLayout.PipelineLayout)
        {
            vkDestroyPipelineLayout(_platform->GetDevice(), _pipelineLayout.PipelineLayout, _platform->GetAllocationCallbacks());
            _pipelineLayout.PipelineLayout = nullptr;
        }

        for (auto& setLayout : _pipelineLayout.DescriptorSetLayouts)
        {
            if (setLayout.DescriptorSetLayout)
            {
                vkDestroyDescriptorSetLayout(_platform->GetDevice(), setLayout.DescriptorSetLayout, _platform->GetAllocationCallbacks());
                setLayout.DescriptorSetLayout = nullptr;
            }
        }

        _pipelineLayout.DescriptorSetLayouts.Clear(true);
        _pipelineLayout.PushConstantRanges.Clear(true);

        if (_shaderModule)
        {
            vkDestroyShaderModule(_platform->GetDevice(), _shaderModule, _platform->GetAllocationCallbacks());
            _shaderModule = nullptr;
        }

        COCO_ENGINE_LOG_VERBOSE("Destroyed VulkanShaderProgram %u", GetID());
    }

    slang::ProgramLayout* VulkanShaderProgram::GetProgramLayout()
    {
        return _linkedProgram->getLayout();
    }

    int64 VulkanShaderProgram::GetParamBlockIndex(const char* name)
    {
        return _globalUniformsLayoutInfo->findFieldIndexByName(name);
    }

    slang::TypeLayoutReflection* VulkanShaderProgram::GetParamBlockLayout(uint64 index)
    {
        auto field = _globalUniformsLayoutInfo->getFieldByIndex(index);
        COCO_ASSERT(field, "Invalid field index");

        return field->getTypeLayout()->getElementTypeLayout();
    }

    VkPipelineVertexInputStateCreateInfo VulkanShaderProgram::GetVertexInputStateCreateInfo() const
    {
        VkPipelineVertexInputStateCreateInfo createInfo{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
        createInfo.pVertexBindingDescriptions = _vertexInputBindingDescriptions.Data();
        createInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(_vertexInputBindingDescriptions.GetCount());
        createInfo.pVertexAttributeDescriptions = _vertexInputAttributeDescriptions.Data();
        createInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(_vertexInputAttributeDescriptions.GetCount());

        return createInfo;
    }

    void VulkanShaderProgram::GetStageInfos(ArrayContainer<VkPipelineShaderStageCreateInfo>& outStageInfos) const
    {
        auto programLayout = _linkedProgram->getLayout();

        for (uint8 i = 0; i < programLayout->getEntryPointCount(); ++i)
        {
            auto entryPoint = programLayout->getEntryPointByIndex(i);
            switch (entryPoint->getStage())
            {
                case SLANG_STAGE_VERTEX:
                {
                    VkPipelineShaderStageCreateInfo& stageInfo = outStageInfos.EmplaceBack(VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO);
                    stageInfo.flags = 0;
                    stageInfo.module = _shaderModule;
                    stageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
                    stageInfo.pName = "vsMain";
                    break;
                }
                case SLANG_STAGE_FRAGMENT:
                {
                    VkPipelineShaderStageCreateInfo& stageInfo = outStageInfos.EmplaceBack(VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO);
                    stageInfo.flags = 0;
                    stageInfo.module = _shaderModule;
                    stageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
                    stageInfo.pName = "psMain";
                    break;
                }
                default:
                    break;
            }
        }
    }

    void VulkanShaderProgram::ReflectVertexInputInformation()
    {
        SlangCompiler::ReflectVertexAttributes(_linkedProgram->getLayout(), _vertexChannels);

        uint32 vertexLocationIndex = 0;

        // Add the vertex attributes
        for (const auto& channel : _vertexChannels)
        {
            VkVertexInputBindingDescription& vertexInput = _vertexInputBindingDescriptions.EmplaceBack();
            vertexInput.binding = vertexLocationIndex; // The index of the binding
            vertexInput.stride = GetVertexChannelElementCount(channel) * sizeof(float); // Size of a single vertice's data
            vertexInput.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // One data entry for each vertex

            VkVertexInputAttributeDescription& desc = _vertexInputAttributeDescriptions.EmplaceBack();
            desc.binding = vertexLocationIndex; // The input binding index
            desc.location = vertexLocationIndex;
            desc.format = VulkanUtils::ToVkFormat(channel);
            desc.offset = 0;

            ++vertexLocationIndex;
        }
    }

    void VulkanShaderProgram::CreatePipelineLayout()
    {
        VulkanPipelineLayoutBuilder pipelineLayoutBuilder(_platform);
        _pipelineLayout = pipelineLayoutBuilder.BuildForProgram(_linkedProgram->getLayout());
    }
} // Coco