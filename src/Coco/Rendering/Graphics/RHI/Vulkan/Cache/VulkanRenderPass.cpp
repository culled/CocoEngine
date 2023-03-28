#include "VulkanRenderPass.h"

#include <Coco/Rendering/Pipeline/RenderPipeline.h>
#include "../GraphicsDeviceVulkan.h"
#include "../VulkanUtilities.h"

namespace Coco::Rendering::Vulkan
{
	VulkanRenderPass::VulkanRenderPass(GraphicsDeviceVulkan* device, const Ref<RenderPipeline>& pipeline) : CachedResource(pipeline->ID, pipeline->GetVersion()),
		_pipelineRef(pipeline), _device(device)
	{}

	VulkanRenderPass::~VulkanRenderPass()
	{
		DestroyRenderPass();
	}

	bool VulkanRenderPass::NeedsUpdate() const noexcept
	{
		if (Ref<RenderPipeline> pipeline = _pipelineRef.lock())
			return _renderPass == nullptr || pipeline->GetVersion() != GetVersion();

		return false;
	}

	void VulkanRenderPass::Update()
	{
		DestroyRenderPass();
		CreateRenderPass();

		Ref<RenderPipeline> pipeline = _pipelineRef.lock();
		UpdateVersion(pipeline->GetVersion());
	}

	void VulkanRenderPass::CreateRenderPass()
	{
		Ref<RenderPipeline> renderPipeline = _pipelineRef.lock();
		if (renderPipeline == nullptr)
			throw VulkanRenderingException("Render pipeline reference was lost");

		List<VkAttachmentDescription> attachments;

		List<RenderPipelineAttachmentDescription> pipelineAttachments = renderPipeline->GetPipelineAttachmentDescriptions();

		// Create attachment descriptions for all pipeline attachments
		for (const RenderPipelineAttachmentDescription& pipelineAttachment : pipelineAttachments)
		{
			VkImageLayout layout = ToAttachmentLayout(pipelineAttachment.Description.PixelFormat);

			VkAttachmentDescription description = {};
			description.samples = VK_SAMPLE_COUNT_1_BIT; // TODO: msaa
			description.format = ToVkFormat(pipelineAttachment.Description.PixelFormat);
			description.loadOp = pipelineAttachment.Description.ShouldPreserve ?
				VK_ATTACHMENT_LOAD_OP_LOAD : VK_ATTACHMENT_LOAD_OP_CLEAR;
			description.storeOp = pipelineAttachment.Description.ShouldPreserve ? 
				VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE;
			description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			description.initialLayout = pipelineAttachment.Description.ShouldPreserve ? 
				layout : VK_IMAGE_LAYOUT_UNDEFINED;
			description.finalLayout = layout;

			attachments.Add(description);
		}

		List<RenderPipelineBinding*> renderPasses = renderPipeline->GetPasses();
		List<VkSubpassDescription> subpasses;
		_subpassInfos.Resize(renderPasses.Count());

		// Create subpasses for all pipeline renderpasses
		for (int i = 0; i < renderPasses.Count(); i++)
		{
			SubpassInfo& subpass = _subpassInfos[i];

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

		AssertVkResult(vkCreateRenderPass(_device->GetDevice(), &createInfo, nullptr, &_renderPass));
	}

	void VulkanRenderPass::DestroyRenderPass()
	{
		if (_renderPass != nullptr)
		{
			_device->WaitForIdle();
			vkDestroyRenderPass(_device->GetDevice(), _renderPass, nullptr);
		}

		_renderPass = nullptr;
	}
}
