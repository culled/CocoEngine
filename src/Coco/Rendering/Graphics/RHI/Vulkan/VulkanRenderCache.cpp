#include "VulkanRenderCache.h"

#include <Coco/Core/Types/Array.h>
#include <Coco/Core/Types/Optional.h>
#include <Coco/Rendering/Pipeline/RenderPipeline.h>
#include <Coco/Rendering/Pipeline/RenderPipelineBinding.h>
#include "GraphicsDeviceVulkan.h"
#include "VulkanUtilities.h"
#include <Coco/Core/Types/List.h>
#include <Coco/Rendering/Mesh.h>
#include "VulkanShader.h"
#include <Coco/Rendering/Graphics/GraphicsPipelineState.h>

namespace Coco::Rendering::Vulkan
{
	const VulkanPipeline VulkanPipeline::None = VulkanPipeline();

	VulkanRenderCache::VulkanRenderCache(GraphicsDeviceVulkan* device) :
		_device(device)
	{
	}

	VulkanRenderCache::~VulkanRenderCache()
	{
		Invalidate();
	}

	void VulkanRenderCache::Invalidate() noexcept
	{
		for (const auto& renderPassKVP : _renderPassCache)
		{
			vkDestroyRenderPass(_device->GetDevice(), renderPassKVP.second.RenderPass, nullptr);
		}

		_renderPassCache.clear();

		for (const auto& pipelineKVP : _pipelineCache)
		{
			vkDestroyPipeline(_device->GetDevice(), pipelineKVP.second.Pipeline, nullptr);
			vkDestroyPipelineLayout(_device->GetDevice(), pipelineKVP.second.Layout, nullptr);
		}

		_pipelineCache.clear();

		for (auto& shaderKVP : _shaderCache)
		{
			shaderKVP.second.reset();
		}

		_shaderCache.clear();
	}

	VulkanRenderPass VulkanRenderCache::GetOrCreateRenderPass(RenderPipeline* renderPipeline)
	{
		const uint64_t key = _renderPipelineHasher(renderPipeline);

		if (!_renderPassCache.contains(key))
		{
			try
			{
				_renderPassCache[key] = CreateRenderPass(renderPipeline);
			}
			catch (const Exception& ex)
			{
				throw VulkanRenderPassCreateException(FormattedString("Failed to create renderpass for pipeline: {}", ex.what()));
			}
		}

		return _renderPassCache[key];
	}

	VulkanPipeline VulkanRenderCache::GetOrCreatePipeline(
		VulkanRenderPass renderPass, 
		const string& subpassName, 
		uint32_t subpassIndex, 
		const Shader* shader,
		VkDescriptorSetLayout globalDescriptorLayout)
	{
		const uint64_t rpHash = _renderPassHasher(renderPass.RenderPass);
		const ResourceID shaderID = shader->GetID();
		const uint64_t key = rpHash ^ (shaderID << 1);

		if (!_pipelineCache.contains(key))
		{
			try
			{
				_pipelineCache[key] = CreatePipeline(renderPass, subpassName, subpassIndex, shader, globalDescriptorLayout);
			}
			catch (const Exception& ex)
			{
				throw VulkanPipelineCreateException(FormattedString(
					"Failed to create pipeline for pass \"{}\" using shader \"{}\": {}",
					subpassName,
					shader->GetName(),
					ex.what()
				));
			}
		}

		return _pipelineCache[key];
	}

	GraphicsResourceRef<VulkanShader> VulkanRenderCache::GetOrCreateVulkanShader(const Shader* shader)
	{
		const ResourceID shaderID = shader->GetID();

		if (!_shaderCache.contains(shaderID))
		{
			try
			{
				_shaderCache[shaderID] = _device->CreateResource<VulkanShader>(shader);
			}
			catch (const Exception& ex)
			{
				throw VulkanShaderCreateException(FormattedString("Failed to create an internal shader for the shader \"{}\": {}",
					shader->GetName(),
					ex.what()
				));
			}
		}

		return _shaderCache[shaderID];
	}

	VulkanRenderPass VulkanRenderCache::CreateRenderPass(RenderPipeline* renderPipeline)
	{
		List<VkAttachmentDescription> attachments;

		List<RenderPipelineAttachmentDescription> pipelineAttachments = renderPipeline->GetPipelineAttachmentDescriptions();

		// Create attachment descriptions for all pipeline attachments
		for (const RenderPipelineAttachmentDescription& pipelineAttachment : pipelineAttachments)
		{
			const VkImageLayout imageLayout = IsDepthStencilFormat(pipelineAttachment.Description.PixelFormat) ? 
				VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			VkAttachmentDescription description = {};
			description.samples = VK_SAMPLE_COUNT_1_BIT; // TODO: msaa
			description.format = ToVkFormat(pipelineAttachment.Description.PixelFormat);
			description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // TODO: temporal effects
			description.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // TODO: temporal effects
			description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // TEMPORARY
			description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // TEMPORARY

			attachments.Add(description);
		}

		List<RenderPipelineBinding*> renderPasses = renderPipeline->GetPasses();
		List<VkSubpassDescription> subpasses;
		VulkanRenderPass renderPass;
		renderPass.Subpasses.Resize(renderPasses.Count());

		// Create subpasses for all pipeline renderpasses
		for (int i = 0; i < renderPasses.Count(); i++)
		{
			SubpassInfo& subpass = renderPass.Subpasses[i];

			// Start assuming no attachments are used
			for (int ii = 0; ii < pipelineAttachments.Count(); ii++)
			{
				subpass.PreserveAttachments.Add(static_cast<uint32_t>(ii));
			}

			List<MappedAttachmentDescription> mappedAttachments = renderPasses[i]->GetMappedAttachmentDescriptions();

			for (int mi = 0; mi < mappedAttachments.Count(); mi++)
			{
				const uint32_t pipelineAttachmentIndex = static_cast<uint32_t>(mappedAttachments[mi].PipelineAttachmentIndex);

				VkAttachmentReference attachmentRef = {};
				attachmentRef.attachment = pipelineAttachmentIndex;

				if (IsDepthStencilFormat(mappedAttachments[mi].AttachmentDescription.PixelFormat) && !subpass.DepthStencilAttachmentReference.has_value())
				{
					attachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
					subpass.DepthStencilAttachmentReference = attachmentRef;
					subpass.DepthStencilAttachment = mappedAttachments[mi].AttachmentDescription;
				}
				else
				{
					attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
					subpass.ColorAttachmentReferences.Add(attachmentRef);
					subpass.ColorAttachments.Add(mappedAttachments[mi].AttachmentDescription);
				}

				subpass.PreserveAttachments.Remove(pipelineAttachmentIndex);
			}

			VkSubpassDescription subpassDescription = {};
			subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpassDescription.colorAttachmentCount = static_cast<uint32_t>(subpass.ColorAttachmentReferences.Count());
			subpassDescription.pColorAttachments = subpass.ColorAttachmentReferences.Data();
			subpassDescription.pDepthStencilAttachment = subpass.DepthStencilAttachmentReference.has_value() ?
				&subpass.DepthStencilAttachmentReference.value() : VK_NULL_HANDLE;
			subpassDescription.preserveAttachmentCount = static_cast<uint32_t>(subpass.PreserveAttachments.Count());
			subpassDescription.pPreserveAttachments = subpass.PreserveAttachments.Data();
			subpassDescription.inputAttachmentCount = 0; // TODO
			subpassDescription.pResolveAttachments = nullptr; // TODO

			subpasses.Add(subpassDescription);
			subpass.SubpassDescription = subpassDescription;
		}

		List<VkSubpassDependency> subpassDependencies;

		// Wait until the previous pipeline operation has completed before performing the vertex stage
		VkSubpassDependency initialDependency = {};
		initialDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		initialDependency.dstSubpass = 0;
		initialDependency.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		initialDependency.dstStageMask = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
		initialDependency.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
		initialDependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		initialDependency.dependencyFlags = 0;

		subpassDependencies.Add(initialDependency);

		for (uint32_t i = 0; i < static_cast<uint32_t>(renderPasses.Count() - 1); i++)
		{
			// Wait until the previous subpass has written to the color attachments before performing the vertex stage on the next pass
			VkSubpassDependency dependency = {};
			dependency.srcSubpass = i;
			dependency.dstSubpass = i + 1;
			dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.dstStageMask = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
			dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			dependency.dependencyFlags = 0;

			subpassDependencies.Add(dependency);
		}

		// Wait until the last subpass finishes writing color attachments before ending the pipeline operation
		VkSubpassDependency finalDependency = {};
		finalDependency.srcSubpass = static_cast<uint32_t>(renderPasses.Count() - 1);
		finalDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
		finalDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		finalDependency.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		finalDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		finalDependency.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		finalDependency.dependencyFlags = 0;

		subpassDependencies.Add(finalDependency);

		VkRenderPassCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		createInfo.attachmentCount = static_cast<uint32_t>(attachments.Count());
		createInfo.pAttachments = attachments.Data();
		createInfo.subpassCount = static_cast<uint32_t>(subpasses.Count());
		createInfo.pSubpasses = subpasses.Data();
		createInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.Count());
		createInfo.pDependencies = subpassDependencies.Data();
		
		AssertVkResult(vkCreateRenderPass(_device->GetDevice(), &createInfo, nullptr, &renderPass.RenderPass));

		return renderPass;
	}

	VulkanPipeline VulkanRenderCache::CreatePipeline(
		VulkanRenderPass renderPass, 
		const string& subpassName, 
		uint32_t subpassIndex, 
		const Shader* shader,
		VkDescriptorSetLayout globalDescriptorLayout)
	{
		const Subshader* subshader;
		if (!shader->TryGetSubshader(subpassName, subshader))
			throw VulkanPipelineCreateException(FormattedString("Could not find a subshader named \"{}\"", subpassName));

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
		rasterizationState.polygonMode = ToVkPolygonMode(subshader->PipelineState.PolygonFillMode);
		rasterizationState.lineWidth = 1.0f;
		rasterizationState.cullMode = ToVkCullModeFlags(subshader->PipelineState.CullingMode);
		rasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizationState.depthClampEnable = subshader->PipelineState.EnableDepthClamping;
		rasterizationState.depthBiasEnable = VK_FALSE; // TODO?
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
		depthStencilState.depthTestEnable = subshader->PipelineState.DepthTestingMode != DepthTestingMode::Never;
		depthStencilState.depthCompareOp = ToVkCompareOp(subshader->PipelineState.DepthTestingMode);
		depthStencilState.depthWriteEnable = subshader->PipelineState.EnableDepthWrite;
		depthStencilState.depthBoundsTestEnable = VK_FALSE;
		depthStencilState.stencilTestEnable = VK_FALSE; // TODO: stencils

		List<VkPipelineColorBlendAttachmentState> attachmentBlendStates;

		// TODO: multiple passes
		List<AttachmentDescription> attachments = renderPass.Subpasses[0].ColorAttachments;

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
		colorBlendState.logicOpEnable = VK_FALSE; // TODO?
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

		for (uint32_t i = 0; i < subshader->Attributes.Count(); i++)
		{
			VkVertexInputAttributeDescription attr = {};
			attr.binding = 0; // The input binding index
			attr.location = i;
			attr.format = ToVkFormat(subshader->Attributes[i].DataFormat);
			attr.offset = subshader->Attributes[i].DataOffset;

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
		inputState.topology = ToVkPrimativeTopology(subshader->PipelineState.TopologyMode);
		inputState.primitiveRestartEnable = VK_FALSE;

		// TODO: make this more configurable
		VkPushConstantRange pushConstants = { };
		pushConstants.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		pushConstants.offset = 0;
		pushConstants.size = sizeof(float) * 16; // 64 bytes for now

		GraphicsResourceRef<VulkanShader> vulkanShader = GetOrCreateVulkanShader(shader);

		VulkanDescriptorLayout layout;
		if(!vulkanShader->TryGetDescriptorSetLayout(subpassName, layout))
			throw VulkanPipelineCreateException("Could not find a descriptor layout for subshader");

		Array<VkDescriptorSetLayout, 2> descriptorSetLayouts = {
			globalDescriptorLayout,
			layout.Layout
		};

		VkPipelineLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		layoutInfo.pSetLayouts = descriptorSetLayouts.data();
		layoutInfo.pushConstantRangeCount = 1;
		layoutInfo.pPushConstantRanges = &pushConstants;

		List<VulkanShaderStage> shaderStages;
		if (!vulkanShader->TryGetSubshaderStages(subpassName, shaderStages))
			throw VulkanPipelineCreateException("Could not find subshader stages");

		VulkanPipeline pipeline = {};
		AssertVkResult(vkCreatePipelineLayout(_device->GetDevice(), &layoutInfo, nullptr, &pipeline.Layout));

		List<VkPipelineShaderStageCreateInfo> shaderStageInfos;

		for (const VulkanShaderStage& stage : shaderStages)
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
		createInfo.pTessellationState = nullptr; // TODO
		createInfo.layout = pipeline.Layout;
		createInfo.renderPass = renderPass.RenderPass;

		createInfo.subpass = subpassIndex;

		createInfo.basePipelineHandle = VK_NULL_HANDLE;
		createInfo.basePipelineIndex = -1;

		AssertVkResult(vkCreateGraphicsPipelines(
			_device->GetDevice(),
			VK_NULL_HANDLE,
			1,
			&createInfo,
			nullptr,
			&pipeline.Pipeline
		));

		return pipeline;
	}
}