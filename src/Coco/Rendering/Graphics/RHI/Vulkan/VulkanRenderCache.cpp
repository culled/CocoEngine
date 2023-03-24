#include "VulkanRenderCache.h"

#include <Coco/Core/Types/Array.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Optional.h>
#include <Coco/Core/Logging/Logger.h>
#include <Coco/Rendering/Pipeline/RenderPipeline.h>
#include <Coco/Rendering/Pipeline/RenderPipelineBinding.h>
#include <Coco/Rendering/Graphics/GraphicsPipelineState.h>
#include <Coco/Rendering/Mesh.h>
#include <Coco/Rendering/Shader.h>
#include "GraphicsDeviceVulkan.h"
#include "VulkanUtilities.h"
#include "VulkanShader.h"
#include "Resources/VulkanDescriptorSet.h"

namespace Coco::Rendering::Vulkan
{
	constexpr uint64_t CombineIDs(uint64_t a, uint64_t b)
	{
		return a ^ (b << 1);
	}

	CachedVulkanRenderPass::CachedVulkanRenderPass(GraphicsDeviceVulkan* device, const Ref<RenderPipeline>& pipeline) : 
		CachedResource(pipeline->ID, pipeline->GetVersion()), Device(device), PipelineRef(pipeline)
	{}

	CachedVulkanRenderPass::~CachedVulkanRenderPass()
	{
		DestroyRenderPass();
	}

	bool CachedVulkanRenderPass::NeedsUpdate() const noexcept
	{
		if (Ref<RenderPipeline> pipeline = PipelineRef.lock())
			return RenderPass == nullptr || pipeline->GetVersion() != this->GetVersion();

		return false;
	}

	void CachedVulkanRenderPass::DestroyRenderPass() noexcept
	{
		if (RenderPass != nullptr)
		{
			Device->WaitForIdle();
			vkDestroyRenderPass(Device->GetDevice(), RenderPass, nullptr);
		}

		RenderPass = nullptr;
	}

	CachedVulkanPipeline::CachedVulkanPipeline(GraphicsDeviceVulkan* device, const Ref<RenderPipeline>& pipeline, const Ref<Shader>& shader) :
		CachedResource(CombineIDs(pipeline->ID, shader->ID), CombineIDs(pipeline->GetVersion(), shader->GetVersion())), 
		Device(device), PipelineRef(pipeline), ShaderRef(shader)
	{}

	CachedVulkanPipeline::~CachedVulkanPipeline()
	{
		DestroyPipeline();
	}

	bool CachedVulkanPipeline::NeedsUpdate() const noexcept
	{
		Ref<RenderPipeline> pipeline = PipelineRef.lock();
		Ref<Shader> shader = ShaderRef.lock();

		if (pipeline != nullptr && shader != nullptr)
			return Pipeline == nullptr || Layout == nullptr || this->GetVersion() != CombineIDs(pipeline->GetVersion(), shader->GetVersion());

		return false;
	}

	void CachedVulkanPipeline::DestroyPipeline() noexcept
	{
		if(Pipeline == nullptr || Layout != nullptr)
			Device->WaitForIdle();

		if (Pipeline != nullptr)
			vkDestroyPipeline(Device->GetDevice(), Pipeline, nullptr);

		Pipeline = nullptr;

		if (Layout != nullptr)
			vkDestroyPipelineLayout(Device->GetDevice(), Layout, nullptr);

		Layout = nullptr;
	}

	VulkanRenderCache::VulkanRenderCache(GraphicsDeviceVulkan* device) :
		_device(device)
	{}

	VulkanRenderCache::~VulkanRenderCache()
	{
		Invalidate();
	}

	void VulkanRenderCache::Invalidate() noexcept
	{
		_renderPassCache.clear();
		_pipelineCache.clear();
		_shaderCache.clear();
	}

	void VulkanRenderCache::PurgeResources() noexcept
	{
		// Purge renderpass cache
		uint64_t renderPassesPurged = 0;
		auto renderPassIt = _renderPassCache.begin();
		while (renderPassIt != _renderPassCache.end())
		{
			const Ref<CachedVulkanRenderPass>& resource = (*renderPassIt).second;

			if (resource->ShouldPurge(s_staleTickCount))
			{
				renderPassIt = _renderPassCache.erase(renderPassIt);
				renderPassesPurged++;
			}
			else
				renderPassIt++;
		}

		// Purge pipeline cache
		uint64_t pipelinesPurged = 0;
		auto pipelineIt = _pipelineCache.begin();
		while (pipelineIt != _pipelineCache.end())
		{
			const Ref<CachedVulkanPipeline>& resource = (*pipelineIt).second;

			if (resource->ShouldPurge(s_staleTickCount))
			{
				pipelineIt = _pipelineCache.erase(pipelineIt);
				pipelinesPurged++;
			}
			else
				pipelineIt++;
		}

		// Purge shader cache
		uint64_t shadersPurged = 0;
		auto shaderIt = _shaderCache.begin();
		while (shaderIt != _shaderCache.end())
		{
			const Ref<VulkanShader>& resource = (*shaderIt).second;

			if (resource->ShouldPurge(s_staleTickCount))
			{
				shaderIt = _shaderCache.erase(shaderIt);
				shadersPurged++;
			}
			else
				shaderIt++;
		}

		if (renderPassesPurged > 0 || pipelinesPurged > 0 || shadersPurged > 0)
			LogTrace(_device->GetLogger(), FormattedString(
				"Purged {} cached renderpasses, {} cached pipelines, and {} cached shaders from global cache",
				renderPassesPurged, 
				pipelinesPurged, 
				shadersPurged
			));
	}

	Ref<CachedVulkanRenderPass> VulkanRenderCache::GetOrCreateRenderPass(const Ref<RenderPipeline>& renderPipeline)
	{
		const ResourceID id = renderPipeline->ID;

		if (!_renderPassCache.contains(id))
			_renderPassCache.emplace(id, CreateRef<CachedVulkanRenderPass>(_device, renderPipeline));

		Ref<CachedVulkanRenderPass>& resource = _renderPassCache.at(id);
		Assert(resource->IsInvalid() == false);

		resource->UpdateTickUsed();

		if (resource->NeedsUpdate())
		{
			LogTrace(_device->GetLogger(), FormattedString("Recreating renderpass for pipeline {}", renderPipeline->ID));

			resource->DestroyRenderPass();

			try
			{
				CreateRenderPass(*resource);
			}
			catch (const Exception& ex)
			{
				throw VulkanRenderingException(FormattedString("Failed to create renderpass for pipeline: {}", ex.what()));
			}
		}

		return resource;
	}

	Ref<CachedVulkanPipeline> VulkanRenderCache::GetOrCreatePipeline(
		const CachedVulkanRenderPass& renderPass,
		const string& subpassName,
		uint32_t subpassIndex,
		const Ref<Shader>& shader,
		const VkDescriptorSetLayout& globalDescriptorLayout)
	{
		Ref<RenderPipeline> pipeline = renderPass.PipelineRef.lock();
		Assert(pipeline != nullptr);

		const uint64_t key = CombineIDs(pipeline->ID, shader->ID);

		if (!_pipelineCache.contains(key))
			_pipelineCache.emplace(key, CreateRef<CachedVulkanPipeline>(_device, pipeline, shader));

		Ref<CachedVulkanPipeline>& resource = _pipelineCache.at(key);
		Assert(resource->IsInvalid() == false);

		resource->UpdateTickUsed();

		if (resource->NeedsUpdate())
		{
			LogTrace(_device->GetLogger(), FormattedString("Recreating pipeline for subpass \"{}\" with shader \"{}\"", subpassName, shader->Name));

			resource->DestroyPipeline();

			try
			{
				CreatePipeline(renderPass, subpassName, subpassIndex, shader, globalDescriptorLayout, *resource);
			}
			catch (const Exception& ex)
			{
				throw VulkanRenderingException(FormattedString(
					"Failed to create pipeline for pass \"{}\" using shader \"{}\": {}",
					subpassName,
					shader->Name,
					ex.what()
				));
			}
		}

		return resource;
	}

	Ref<VulkanShader> VulkanRenderCache::GetOrCreateVulkanShader(const Ref<Shader>& shader)
	{
		const ResourceID id = shader->ID;

		if (!_shaderCache.contains(id))
			_shaderCache.emplace(id, CreateRef<VulkanShader>(_device, shader));

		Ref<VulkanShader>& resource = _shaderCache.at(id);
		Assert(resource->IsInvalid() == false);

		resource->UpdateTickUsed();

		if (resource->NeedsUpdate())
		{
			LogTrace(_device->GetLogger(), FormattedString("Recreating Vulkan shader for shader \"{}\"", shader->Name));

			try
			{
				resource->Update();
			}
			catch (const Exception& ex)
			{
				throw VulkanRenderingException(FormattedString("Failed to create an internal shader for the shader \"{}\": {}",
					shader->Name,
					ex.what()
				));
			}
		}

		return resource;
	}

	void VulkanRenderCache::CreateRenderPass(CachedVulkanRenderPass& cachedRenderPass)
	{
		Ref<RenderPipeline> renderPipeline = cachedRenderPass.PipelineRef.lock();
		if (renderPipeline == nullptr)
			throw VulkanRenderingException("Render pipeline was empty");

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
		cachedRenderPass.Subpasses.Resize(renderPasses.Count());

		// Create subpasses for all pipeline renderpasses
		for (int i = 0; i < renderPasses.Count(); i++)
		{
			SubpassInfo& subpass = cachedRenderPass.Subpasses[i];

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
			subpassDescription.inputAttachmentCount = 0; // TODO: input attachments
			subpassDescription.pResolveAttachments = nullptr; // TODO: resolve attachments

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
		
		AssertVkResult(vkCreateRenderPass(_device->GetDevice(), &createInfo, nullptr, &cachedRenderPass.RenderPass));
	}

	void VulkanRenderCache::CreatePipeline(
		const CachedVulkanRenderPass& renderPass, 
		const string& subpassName, 
		uint32_t subpassIndex, 
		const Ref<Shader>& shader,
		const VkDescriptorSetLayout& globalDescriptorLayout,
		CachedVulkanPipeline& cachedPipeline)
	{
		const Subshader* subshader;
		if (!shader->TryGetSubshader(subpassName, subshader))
			throw VulkanRenderingException(FormattedString("Could not find a subshader named \"{}\"", subpassName));

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
		depthStencilState.depthTestEnable = subshader->PipelineState.DepthTestingMode != DepthTestingMode::Never;
		depthStencilState.depthCompareOp = ToVkCompareOp(subshader->PipelineState.DepthTestingMode);
		depthStencilState.depthWriteEnable = subshader->PipelineState.EnableDepthWrite;
		depthStencilState.depthBoundsTestEnable = VK_FALSE;
		depthStencilState.stencilTestEnable = VK_FALSE; // TODO: stencils

		List<VkPipelineColorBlendAttachmentState> attachmentBlendStates;

		const List<AttachmentDescription>& attachments = renderPass.Subpasses[subpassIndex].ColorAttachments;

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

		for (uint32_t i = 0; i < subshader->Attributes.Count(); i++)
		{
			VkVertexInputAttributeDescription attr = {};
			attr.binding = 0; // The input binding index
			attr.location = i;
			attr.format = ToVkFormat(subshader->Attributes[i].DataFormat);
			attr.offset = subshader->Attributes[i].GetDataOffset();

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

		// TODO: make push constantes more configurable
		VkPushConstantRange pushConstants = { };
		pushConstants.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		pushConstants.offset = 0;
		pushConstants.size = sizeof(float) * 16; // 64 bytes for now

		Ref<VulkanShader> vulkanShader = GetOrCreateVulkanShader(shader);

		const VulkanSubshader* vulkanSubshader;
		if(!vulkanShader->TryGetSubshader(subpassName, vulkanSubshader))
			throw VulkanRenderingException(FormattedString("Could not find a Vulkan subshader for subshader \"{}\"", subpassName));

		Array<VkDescriptorSetLayout, 2> descriptorSetLayouts = {
			globalDescriptorLayout,
			vulkanSubshader->DescriptorLayout.Layout
		};

		VkPipelineLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		layoutInfo.pSetLayouts = descriptorSetLayouts.data();
		layoutInfo.pushConstantRangeCount = 1;
		layoutInfo.pPushConstantRanges = &pushConstants;

		AssertVkResult(vkCreatePipelineLayout(_device->GetDevice(), &layoutInfo, nullptr, &cachedPipeline.Layout));

		List<VkPipelineShaderStageCreateInfo> shaderStageInfos;

		for (const VulkanShaderStage& stage : vulkanSubshader->ShaderStages)
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
		createInfo.layout = cachedPipeline.Layout;
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
			&cachedPipeline.Pipeline
		));
	}
}