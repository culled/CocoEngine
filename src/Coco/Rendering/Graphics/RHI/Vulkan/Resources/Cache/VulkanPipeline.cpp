#include "VulkanPipeline.h"
#include <Coco/Core/Math/Math.h>

#include <Coco/Core/Types/Array.h>
#include "../../GraphicsDeviceVulkan.h"
#include "../../VulkanUtilities.h"
#include <Coco/Rendering/Pipeline/RenderPipeline.h>
#include <Coco/Rendering/Graphics/RenderView.h>
#include <Coco/Rendering/Mesh.h>
#include "VulkanShader.h"
#include "VulkanRenderPass.h"

namespace Coco::Rendering::Vulkan
{
	VulkanPipeline::VulkanPipeline(
		const ResourceID& id,
		const string& name,
		const VulkanRenderPass& renderPass,
		const VulkanShader& shader,
		const string& subshaderName,
		uint subpassIndex) :
		GraphicsResource<GraphicsDeviceVulkan, RenderingResource>(id, name),
		CachedResource(GetResourceID(renderPass, shader), GetResourceVersion(renderPass, shader)),
		_subshaderName(subshaderName), _subpassIndex(subpassIndex)
	{}

	VulkanPipeline::~VulkanPipeline()
	{
		DestroyPipeline();
	}

	bool VulkanPipeline::NeedsUpdate(const VulkanRenderPass& renderPass, const VulkanShader& shader) const noexcept
	{
		return _pipeline == nullptr || _pipelineLayout == nullptr || GetReferenceVersion() != GetResourceVersion(renderPass, shader);
	}

	void VulkanPipeline::Update(VulkanRenderPass& renderPass, const VulkanShader& shader)
	{
		DestroyPipeline();
		CreatePipeline(renderPass, shader);

		UpdateReferenceVersion(GetResourceVersion(renderPass, shader));
		IncrementVersion();
	}

	void VulkanPipeline::CreatePipeline(VulkanRenderPass& renderPass, const VulkanShader& shader)
	{
		const VulkanSubshader* vulkanSubshader;
		if (!shader.TryGetSubshader(_subshaderName, vulkanSubshader))
			throw RenderingException(FormattedString("Shader {} has no subshader named {}", shader.GetName(), _subshaderName));

		// Early out if no valid stages
		if (vulkanSubshader->GetStages().Count() == 0)
			throw RenderingException(FormattedString("Subshader {} of shader {} has no valid shader stages", _subshaderName, shader.GetName()));

		const Subshader& subshader = vulkanSubshader->GetSubshader();

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
		rasterizationState.polygonMode = ToVkPolygonMode(subshader.PipelineState.PolygonFillMode);
		rasterizationState.lineWidth = 1.0f;
		rasterizationState.cullMode = ToVkCullModeFlags(subshader.PipelineState.CullingMode);
		rasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizationState.depthClampEnable = subshader.PipelineState.EnableDepthClamping;
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
		depthStencilState.depthTestEnable = subshader.PipelineState.DepthTestingMode != DepthTestingMode::Never;
		depthStencilState.depthCompareOp = ToVkCompareOp(subshader.PipelineState.DepthTestingMode);
		depthStencilState.depthWriteEnable = subshader.PipelineState.EnableDepthWrite;
		depthStencilState.depthBoundsTestEnable = VK_FALSE;
		depthStencilState.stencilTestEnable = VK_FALSE; // TODO: stencils

		List<VkPipelineColorBlendAttachmentState> attachmentBlendStates;

		const List<AttachmentDescription>& attachments = renderPass.GetSubpassInfo(_subpassIndex).ColorAttachments;

		for (uint64_t i = 0; i < attachments.Count(); i++)
		{
			VkPipelineColorBlendAttachmentState blendState = {};
			blendState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			blendState.blendEnable = attachments[i].ColorBlendOperation != BlendOperation::None || 
				attachments[i].AlphaBlendOperation != BlendOperation::None;
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

		for (uint32_t i = 0; i < subshader.Attributes.Count(); i++)
		{
			VkVertexInputAttributeDescription attr = {};
			attr.binding = 0; // The input binding index
			attr.location = i;
			attr.format = ToVkFormat(subshader.Attributes[i].DataFormat);
			attr.offset = subshader.Attributes[i].GetDataOffset();

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
		inputState.topology = ToVkPrimativeTopology(subshader.PipelineState.TopologyMode);
		inputState.primitiveRestartEnable = VK_FALSE;

		List<VkDescriptorSetLayout> descriptorSetLayouts = vulkanSubshader->GetDescriptorLayouts().Transform<VkDescriptorSetLayout>(
			[](const VulkanDescriptorLayout& e) {return e.Layout; }
		);

		List<VkPushConstantRange> pushConstantRanges = vulkanSubshader->GetPushConstantRanges();

		VkPipelineLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.Count());
		layoutInfo.pSetLayouts = descriptorSetLayouts.Data();
		layoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.Count());
		layoutInfo.pPushConstantRanges = pushConstantRanges.Data();

		AssertVkResult(vkCreatePipelineLayout(_device->GetDevice(), &layoutInfo, nullptr, &_pipelineLayout));

		List<VkPipelineShaderStageCreateInfo> shaderStageInfos;

		for (const VulkanShaderStage& stage : vulkanSubshader->GetStages())
		{
			VkPipelineShaderStageCreateInfo stageInfo = {};
			stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			stageInfo.stage = ToVkShaderStageFlagBits(stage.Type);
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
		createInfo.renderPass = renderPass.GetRenderPass();

		createInfo.subpass = _subpassIndex;

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

	ResourceID VulkanPipeline::GetResourceID(const VulkanRenderPass& pass, const VulkanShader& shader)
	{
		return UUID::Combine(pass.ReferenceID, shader.ReferenceID);
	}

	ResourceVersion VulkanPipeline::GetResourceVersion(const VulkanRenderPass& pass, const VulkanShader& shader)
	{
		return Math::CombineHashes(0, pass.GetReferenceVersion(), shader.GetReferenceVersion());
	}
}
