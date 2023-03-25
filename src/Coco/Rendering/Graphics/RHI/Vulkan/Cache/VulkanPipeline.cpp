#include "VulkanPipeline.h"
#include <Coco/Core/Math/Math.h>

#include <Coco/Core/Types/Array.h>
#include "../GraphicsDeviceVulkan.h"
#include "../VulkanUtilities.h"
#include <Coco/Rendering/Pipeline/RenderPipeline.h>
#include <Coco/Rendering/Graphics/RenderView.h>
#include <Coco/Rendering/Mesh.h>
#include "VulkanSubshader.h"
#include "VulkanRenderPass.h"

namespace Coco::Rendering::Vulkan
{
	VulkanPipeline::VulkanPipeline(
		GraphicsDeviceVulkan* device, 
		const Ref<VulkanRenderPass>& renderPass, 
		const Ref<VulkanSubshader>& subshader, 
		uint subpassIndex) :
		CachedResource(Math::CombineHashes(0, renderPass->ID, subshader->ShaderID), Math::CombineHashes(0, renderPass->GetVersion(), subshader->GetVersion())), 
		_device(device), _renderPassRef(renderPass), _subshaderRef(subshader), SubpassIndex(subpassIndex)
	{}

	VulkanPipeline::~VulkanPipeline()
	{
		DestroyPipeline();
	}

	bool VulkanPipeline::NeedsUpdate() const noexcept
	{
		Ref<VulkanRenderPass> renderPass = _renderPassRef.lock();
		Ref<VulkanSubshader> subshader = _subshaderRef.lock();

		if(renderPass != nullptr && subshader != nullptr)
			return _pipeline == nullptr || _pipelineLayout == nullptr || GetVersion() != Math::CombineHashes(0, renderPass->GetVersion(), subshader->GetVersion());

		return false;
	}

	void VulkanPipeline::Update(const VkDescriptorSetLayout& globalDescriptorLayout)
	{
		DestroyPipeline();

		CreatePipeline(globalDescriptorLayout);

		Ref<VulkanRenderPass> renderPass = _renderPassRef.lock();
		Ref<VulkanSubshader> subshader = _subshaderRef.lock();
		UpdateVersion(Math::CombineHashes(0, renderPass->GetVersion(), subshader->GetVersion()));
	}

	void VulkanPipeline::CreatePipeline(const VkDescriptorSetLayout& globalDescriptorLayout)
	{
		Ref<VulkanRenderPass> renderPass = _renderPassRef.lock();
		if(renderPass == nullptr)
			throw VulkanRenderingException("RenderPass reference was lost");

		Ref<VulkanSubshader> subshader = _subshaderRef.lock();
		if (subshader == nullptr)
			throw VulkanRenderingException("Subshader reference was lost");

		const Subshader& subshaderInfo = subshader->GetSubshader();

		List<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_LINE_WIDTH };

		VkPipelineDynamicStateCreateInfo dynamicState = {};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.Count());
		dynamicState.pDynamicStates = dynamicStates.Data();

		// TODO: multiple viewports and scissors
		// Only need to specify the number of viewports and scissors since they are dynamic states
		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo rasterizationState = {};
		rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationState.rasterizerDiscardEnable = VK_FALSE;
		rasterizationState.polygonMode = ToVkPolygonMode(subshaderInfo.PipelineState.PolygonFillMode);
		rasterizationState.lineWidth = 1.0f;
		rasterizationState.cullMode = ToVkCullModeFlags(subshaderInfo.PipelineState.CullingMode);
		rasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizationState.depthClampEnable = subshaderInfo.PipelineState.EnableDepthClamping;
		rasterizationState.depthBiasEnable = VK_FALSE; // TODO: depth biasing?
		rasterizationState.depthBiasConstantFactor = 0.0f;
		rasterizationState.depthBiasSlopeFactor = 0.0f;
		rasterizationState.depthBiasClamp = 0.0f;

		// TODO: multisampling
		VkPipelineMultisampleStateCreateInfo multisampleState = {};
		multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleState.sampleShadingEnable = VK_FALSE;
		multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampleState.minSampleShading = 1.0f;
		multisampleState.pSampleMask = nullptr;
		multisampleState.alphaToCoverageEnable = VK_FALSE;
		multisampleState.alphaToOneEnable = VK_FALSE;

		VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
		depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilState.depthTestEnable = subshaderInfo.PipelineState.DepthTestingMode != DepthTestingMode::Never;
		depthStencilState.depthCompareOp = ToVkCompareOp(subshaderInfo.PipelineState.DepthTestingMode);
		depthStencilState.depthWriteEnable = subshaderInfo.PipelineState.EnableDepthWrite;
		depthStencilState.depthBoundsTestEnable = VK_FALSE;
		depthStencilState.stencilTestEnable = VK_FALSE; // TODO: stencils

		List<VkPipelineColorBlendAttachmentState> attachmentBlendStates;

		const List<AttachmentDescription>& attachments = renderPass->GetSubpassInfo(SubpassIndex).ColorAttachments;

		for (uint64_t i = 0; i < attachments.Count(); i++)
		{
			VkPipelineColorBlendAttachmentState blendState = {};
			blendState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			blendState.blendEnable = attachments[i].UsesBlending;
			blendState.srcColorBlendFactor = ToVkBlendFactor(attachments[i].ColorSourceFactor);
			blendState.dstColorBlendFactor = ToVkBlendFactor(attachments[i].ColorDestinationFactor);
			blendState.colorBlendOp = ToVkBlendOp(attachments[i].ColorBlendOperation);
			blendState.srcAlphaBlendFactor = ToVkBlendFactor(attachments[i].AlphaSourceBlendFactor);
			blendState.dstAlphaBlendFactor = ToVkBlendFactor(attachments[i].AlphaDestinationBlendFactor);
			blendState.alphaBlendOp = ToVkBlendOp(attachments[i].AlphaBlendOperation);

			attachmentBlendStates.Add(blendState);
		}

		VkPipelineColorBlendStateCreateInfo colorBlendState = {};
		colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendState.attachmentCount = static_cast<uint32_t>(attachmentBlendStates.Count());
		colorBlendState.pAttachments = attachmentBlendStates.Data();
		colorBlendState.logicOpEnable = VK_FALSE; // TODO: logic blending?
		colorBlendState.logicOp = VK_LOGIC_OP_COPY;
		colorBlendState.blendConstants[0] = 0.0f;
		colorBlendState.blendConstants[1] = 0.0f;
		colorBlendState.blendConstants[2] = 0.0f;
		colorBlendState.blendConstants[3] = 0.0f;

		VkVertexInputBindingDescription vertexInput = {};
		vertexInput.binding = 0; // The index of the binding
		vertexInput.stride = sizeof(VertexData);
		vertexInput.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // One data entry for each vertex

		List<VkVertexInputBindingDescription> vertexInputs = { vertexInput };

		List<VkVertexInputAttributeDescription> vertexAttributes;

		for (uint32_t i = 0; i < subshaderInfo.Attributes.Count(); i++)
		{
			VkVertexInputAttributeDescription attr = {};
			attr.binding = 0; // The input binding index
			attr.location = i;
			attr.format = ToVkFormat(subshaderInfo.Attributes[i].DataFormat);
			attr.offset = subshaderInfo.Attributes[i].GetDataOffset();

			vertexAttributes.Add(attr);
		}

		VkPipelineVertexInputStateCreateInfo vertexInputState = {};
		vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputState.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexInputs.Count());
		vertexInputState.pVertexBindingDescriptions = vertexInputs.Data();
		vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributes.Count());
		vertexInputState.pVertexAttributeDescriptions = vertexAttributes.Data();

		VkPipelineInputAssemblyStateCreateInfo inputState = {};
		inputState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputState.topology = ToVkPrimativeTopology(subshaderInfo.PipelineState.TopologyMode);
		inputState.primitiveRestartEnable = VK_FALSE;

		// TODO: make push constantes more configurable
		VkPushConstantRange pushConstants = { };
		pushConstants.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		pushConstants.offset = 0;
		pushConstants.size = sizeof(float) * 16; // 64 bytes for now

		Array<VkDescriptorSetLayout, 2> descriptorSetLayouts = {
			globalDescriptorLayout,
			subshader->GetDescriptorLayout().Layout
		};

		VkPipelineLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		layoutInfo.pSetLayouts = descriptorSetLayouts.data();
		layoutInfo.pushConstantRangeCount = 1;
		layoutInfo.pPushConstantRanges = &pushConstants;

		AssertVkResult(vkCreatePipelineLayout(_device->GetDevice(), &layoutInfo, nullptr, &_pipelineLayout));

		List<VkPipelineShaderStageCreateInfo> shaderStageInfos;

		for (const VulkanShaderStage& stage : subshader->GetStages())
		{
			VkPipelineShaderStageCreateInfo stageInfo = {};
			stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			stageInfo.stage = ToVkShaderStageFlagBits(stage.StageType);
			stageInfo.module = stage.ShaderModule;
			stageInfo.pName = stage.EntryPointName.c_str();

			shaderStageInfos.Add(stageInfo);
		}

		VkGraphicsPipelineCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		createInfo.stageCount = static_cast<uint32_t>(shaderStageInfos.Count());
		createInfo.pStages = shaderStageInfos.Data();
		createInfo.pVertexInputState = &vertexInputState;
		createInfo.pInputAssemblyState = &inputState;
		createInfo.pViewportState = &viewportState;
		createInfo.pRasterizationState = &rasterizationState;
		createInfo.pMultisampleState = &multisampleState;
		createInfo.pDepthStencilState = &depthStencilState;
		createInfo.pColorBlendState = &colorBlendState;
		createInfo.pDynamicState = &dynamicState;
		createInfo.pTessellationState = nullptr; // TODO: tessellation
		createInfo.layout = _pipelineLayout;
		createInfo.renderPass = renderPass->GetRenderPass();

		createInfo.subpass = SubpassIndex;

		createInfo.basePipelineHandle = VK_NULL_HANDLE;
		createInfo.basePipelineIndex = -1;

		AssertVkResult(vkCreateGraphicsPipelines(
			_device->GetDevice(),
			VK_NULL_HANDLE,
			1,
			&createInfo,
			nullptr,
			&_pipeline
		));
	}

	void VulkanPipeline::DestroyPipeline() noexcept
	{
		if (_pipeline != nullptr || _pipelineLayout != nullptr)
			_device->WaitForIdle();

		if (_pipeline != nullptr)
			vkDestroyPipeline(_device->GetDevice(), _pipeline, nullptr);

		if (_pipelineLayout != nullptr)
			vkDestroyPipelineLayout(_device->GetDevice(), _pipelineLayout, nullptr);

		_pipeline = nullptr;
		_pipelineLayout = nullptr;
	}
}
