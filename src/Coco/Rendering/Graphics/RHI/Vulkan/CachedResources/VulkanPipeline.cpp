#include "Renderpch.h"
#include "VulkanPipeline.h"
#include "VulkanRenderPass.h"
#include "VulkanRenderPassShader.h"
#include "../VulkanGraphicsDevice.h"
#include "../VulkanUtils.h"
#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	VulkanPipeline::VulkanPipeline(
		const VulkanRenderPass& renderPass,
		const VulkanRenderPassShader& shader,
		uint32 subpassIndex,
		const VulkanDescriptorSetLayout* globalLayout) :
		CachedVulkanResource(MakeKey(renderPass, shader, subpassIndex, globalLayout)),
		_version(0),
		_subpassIndex(subpassIndex),
		_pipelineLayout(nullptr),
		_pipeline(nullptr)
	{}

	VulkanPipeline::~VulkanPipeline()
	{
		DestroyPipeline();
	}

	GraphicsDeviceResourceID VulkanPipeline::MakeKey(
		const VulkanRenderPass& renderPass, 
		const VulkanRenderPassShader& shader, 
		uint32 subpassIndex,
		const VulkanDescriptorSetLayout* globalLayout)
	{
		return Math::CombineHashes(globalLayout ? globalLayout->LayoutHash : 0, renderPass.ID, shader.ID, subpassIndex);
	}

	bool VulkanPipeline::NeedsUpdate(
		const VulkanRenderPass& renderPass, 
		const VulkanRenderPassShader& shader)
	{
		return _pipeline == nullptr || MakeVersion(renderPass, shader) != _version;
	}

	void VulkanPipeline::Update(
		const VulkanRenderPass& renderPass, 
		const VulkanRenderPassShader& shader, 
		uint32 subpassIndex,
		const VulkanDescriptorSetLayout* globalLayout)
	{
		DestroyPipeline();
		CreatePipeline(renderPass, shader, subpassIndex, globalLayout);

		_version = MakeVersion(renderPass, shader);
	}

	uint64 VulkanPipeline::MakeVersion(const VulkanRenderPass& renderPass, const VulkanRenderPassShader& shader)
	{
		return Math::CombineHashes(renderPass.GetVersion(), shader.GetVersion());
	}

	void VulkanPipeline::CreatePipeline(
		const VulkanRenderPass& renderPass, 
		const VulkanRenderPassShader& shader, 
		uint32 subpassIndex,
		const VulkanDescriptorSetLayout* globalLayout)
	{
		std::vector<VulkanDescriptorSetLayout> shaderLayouts = shader.GetDescriptorSetLayouts();

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts;

		std::transform(shaderLayouts.begin(), shaderLayouts.end(),
			std::back_inserter(descriptorSetLayouts),
			[](const VulkanDescriptorSetLayout& layout) { return layout.Layout; });

		if (globalLayout)
			descriptorSetLayouts.emplace(descriptorSetLayouts.begin(), globalLayout->Layout);

		std::vector<VkPushConstantRange> pushConstantRanges = shader.GetPushConstantRanges();

		// Pipeline layout
		VkPipelineLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		layoutInfo.pSetLayouts = descriptorSetLayouts.data();
		layoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
		layoutInfo.pPushConstantRanges = pushConstantRanges.size() > 0 ? pushConstantRanges.data() : nullptr;

		AssertVkSuccess(vkCreatePipelineLayout(_device.GetDevice(), &layoutInfo, _device.GetAllocationCallbacks(), &_pipelineLayout));

		// Setup dynamic state
		std::array<VkDynamicState, 3> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_LINE_WIDTH };

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		// TODO: multiple viewports and scissors
		// Only need to specify the number of viewports and scissors since they are dynamic states
		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		const RenderPassShader& shaderInfo = shader.GetInfo();
		const GraphicsPipelineState& pipelineState = shaderInfo.PipelineState;

		// Rasterization state
		VkPipelineRasterizationStateCreateInfo rasterizationState{};
		rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationState.rasterizerDiscardEnable = VK_FALSE;
		rasterizationState.polygonMode = ToVkPolygonMode(pipelineState.PolygonFillMode);
		rasterizationState.lineWidth = 1.0f;
		rasterizationState.cullMode = ToVkCullModeFlags(pipelineState.CullingMode);
		rasterizationState.frontFace = ToVkFrontFace(pipelineState.WindingMode);
		rasterizationState.depthClampEnable = pipelineState.EnableDepthClamping;
		rasterizationState.depthBiasEnable = VK_FALSE; // TODO: depth biasing?
		rasterizationState.depthBiasConstantFactor = 0.0f;
		rasterizationState.depthBiasSlopeFactor = 0.0f;
		rasterizationState.depthBiasClamp = 0.0f;

		// Multisample state
		VkPipelineMultisampleStateCreateInfo multisampleState{};
		multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleState.sampleShadingEnable = VK_FALSE;
		multisampleState.rasterizationSamples = ToVkSampleCountFlagBits(renderPass.GetMSAASamples());
		multisampleState.minSampleShading = 1.0f;
		multisampleState.pSampleMask = nullptr;
		multisampleState.alphaToCoverageEnable = VK_FALSE;
		multisampleState.alphaToOneEnable = VK_FALSE;

		// Depth/stencil state
		VkPipelineDepthStencilStateCreateInfo depthStencilState{};
		depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilState.depthTestEnable = pipelineState.DepthTestingMode != DepthTestingMode::Never;
		depthStencilState.depthCompareOp = ToVkCompareOp(pipelineState.DepthTestingMode);
		depthStencilState.depthWriteEnable = pipelineState.EnableDepthWrite;
		depthStencilState.depthBoundsTestEnable = VK_FALSE;
		depthStencilState.stencilTestEnable = VK_FALSE; // TODO: stencils

		// Blend states
		std::vector<VkPipelineColorBlendAttachmentState> attachmentBlendStates(shaderInfo.AttachmentBlendStates.size());
		for (uint64_t i = 0; i < attachmentBlendStates.size(); i++)
		{
			const BlendState& state = shaderInfo.AttachmentBlendStates.at(i);
			VkPipelineColorBlendAttachmentState& blendState = attachmentBlendStates.at(i);
			blendState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

			blendState.blendEnable = state.ColorBlendOperation != BlendOperation::None || state.AlphaBlendOperation != BlendOperation::None;
			blendState.srcColorBlendFactor = ToVkBlendFactor(state.ColorSourceFactor);
			blendState.dstColorBlendFactor = ToVkBlendFactor(state.ColorDestinationFactor);
			blendState.colorBlendOp = ToVkBlendOp(state.ColorBlendOperation);
			blendState.srcAlphaBlendFactor = ToVkBlendFactor(state.AlphaSourceFactor);
			blendState.dstAlphaBlendFactor = ToVkBlendFactor(state.AlphaDestinationFactor);
			blendState.alphaBlendOp = ToVkBlendOp(state.AlphaBlendOperation);
		}

		VkPipelineColorBlendStateCreateInfo colorBlendState{};
		colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendState.attachmentCount = static_cast<uint32_t>(attachmentBlendStates.size());
		colorBlendState.pAttachments = attachmentBlendStates.data();
		colorBlendState.logicOpEnable = VK_FALSE; // TODO: logic blending?
		colorBlendState.logicOp = VK_LOGIC_OP_COPY;
		colorBlendState.blendConstants[0] = 0.0f;
		colorBlendState.blendConstants[1] = 0.0f;
		colorBlendState.blendConstants[2] = 0.0f;
		colorBlendState.blendConstants[3] = 0.0f;

		// Attribute inputs
		VkVertexInputBindingDescription vertexInput{};
		vertexInput.binding = 0; // The index of the binding
		vertexInput.stride = shaderInfo.VertexDataSize;
		vertexInput.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // One data entry for each vertex

		std::array<VkVertexInputBindingDescription, 1> vertexInputs = { vertexInput };

		std::vector<VkVertexInputAttributeDescription> vertexAttributes(shaderInfo.VertexAttributes.size());
		for (uint32_t i = 0; i < vertexAttributes.size(); i++)
		{
			const ShaderVertexAttribute& attr = shaderInfo.VertexAttributes.at(i);
			VkVertexInputAttributeDescription& attrDescription = vertexAttributes.at(i);
			attrDescription.binding = 0; // The input binding index
			attrDescription.location = i;
			attrDescription.format = ToVkFormat(attr.Type);
			attrDescription.offset = attr.Offset;
		}

		VkPipelineVertexInputStateCreateInfo vertexInputState{};
		vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputState.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexInputs.size());
		vertexInputState.pVertexBindingDescriptions = vertexInputs.data();
		vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributes.size());
		vertexInputState.pVertexAttributeDescriptions = vertexAttributes.data();

		// Input assembly state
		VkPipelineInputAssemblyStateCreateInfo inputState{};
		inputState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputState.topology = ToVkPrimitiveTopology(pipelineState.TopologyMode);
		inputState.primitiveRestartEnable = VK_FALSE;

		std::vector<VkPipelineShaderStageCreateInfo> shaderStageInfos;

		for (const VulkanShaderStage& stage : shader.GetStages())
		{
			VkPipelineShaderStageCreateInfo stageInfo{};
			stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			stageInfo.stage = ToVkShaderStageFlagBits(stage.Type);
			stageInfo.module = stage.ShaderModule;
			stageInfo.pName = stage.EntryPointName.c_str();

			shaderStageInfos.push_back(stageInfo);
		}

		VkGraphicsPipelineCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		createInfo.stageCount = static_cast<uint32_t>(shaderStageInfos.size());
		createInfo.pStages = shaderStageInfos.data();
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
		createInfo.renderPass = renderPass.GetRenderPass();
		createInfo.subpass = _subpassIndex;
		createInfo.basePipelineHandle = VK_NULL_HANDLE;
		createInfo.basePipelineIndex = -1;

		AssertVkSuccess(vkCreateGraphicsPipelines(
			_device.GetDevice(),
			VK_NULL_HANDLE,
			1,
			&createInfo,
			_device.GetAllocationCallbacks(),
			&_pipeline
		));

		CocoTrace("Created VulkanPipeline")
	}

	void VulkanPipeline::DestroyPipeline()
	{
		if (_pipeline || _pipelineLayout)
		{
			_device.WaitForIdle();

			if (_pipeline)
			{
				vkDestroyPipeline(_device.GetDevice(), _pipeline, _device.GetAllocationCallbacks());
				_pipeline = nullptr;
			}

			if (_pipelineLayout)
			{
				vkDestroyPipelineLayout(_device.GetDevice(), _pipelineLayout, _device.GetAllocationCallbacks());
				_pipelineLayout = nullptr;
			}

			CocoTrace("Destroyed VulkanPipeline")
		}
	}
}