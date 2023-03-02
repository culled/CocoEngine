#include "VulkanRenderCache.h"

#include <Coco/Core/Types/Optional.h>
#include <Coco/Rendering/Pipeline/RenderPipeline.h>
#include <Coco/Rendering/Pipeline/RenderPipelineBinding.h>
#include "GraphicsDeviceVulkan.h"
#include "VulkanUtilities.h"
#include <Coco/Rendering/Graphics/ImageUtilities.h>

namespace Coco::Rendering
{
	VulkanRenderCache::VulkanRenderCache(GraphicsDeviceVulkan* device) :
		_device(device)
	{
	}

	VulkanRenderCache::~VulkanRenderCache()
	{
		Invalidate();
	}

	void VulkanRenderCache::Invalidate()
	{
		for (const auto& renderPassKVP : _renderPassCache)
		{
			vkDestroyRenderPass(_device->GetDevice(), renderPassKVP.second, nullptr);
		}

		_renderPassCache.clear();
	}

	VkRenderPass VulkanRenderCache::GetOrCreateRenderPass(const Ref<RenderPipeline>& renderPipeline)
	{
		if (!_renderPassCache.contains(renderPipeline))
		{
			_renderPassCache[renderPipeline] = CreateRenderPass(renderPipeline);
		}
		
		return _renderPassCache[renderPipeline];
	}

	VkRenderPass VulkanRenderCache::CreateRenderPass(const Ref<RenderPipeline>& renderPipeline)
	{
		List<VkAttachmentDescription> attachments;

		List<RenderPipelineAttachmentDescription> pipelineAttachments = renderPipeline->GetPipelineAttachmentDescriptions();

		// Create attachment descriptions for all pipeline attachments
		for (const RenderPipelineAttachmentDescription& pipelineAttachment : pipelineAttachments)
		{
			VkImageLayout imageLayout = IsDepthStencilFormat(pipelineAttachment.Description.PixelFormat) ? 
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
		List<SubpassInfo> subpassInfos(renderPasses.Count());

		// Create subpasses for all pipeline renderpasses
		for (int i = 0; i < renderPasses.Count(); i++)
		{
			// Start assuming no attachments are used
			for (int ii = 0; ii < pipelineAttachments.Count(); ii++)
			{
				subpassInfos[i].PreserveAttachments.Add(static_cast<uint32_t>(ii));
			}

			List<MappedAttachmentDescription> mappedAttachments = renderPasses[i]->GetMappedAttachmentDescriptions();

			for (int mi = 0; mi < mappedAttachments.Count(); mi++)
			{
				uint32_t pipelineAttachmentIndex = static_cast<uint32_t>(mappedAttachments[mi].PipelineAttachmentIndex);

				VkAttachmentReference attachmentRef = {};
				attachmentRef.attachment = pipelineAttachmentIndex;

				if (IsDepthStencilFormat(mappedAttachments[mi].AttachmentDescription.PixelFormat))
				{
					if (subpassInfos[i].DepthStencilAttachmentReference.has_value())
						continue;

					attachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
					subpassInfos[i].DepthStencilAttachmentReference = attachmentRef;
				}
				else
				{
					attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
					subpassInfos[i].ColorAttachmentReferences.Add(attachmentRef);
				}

				subpassInfos[i].PreserveAttachments.Remove(pipelineAttachmentIndex);
			}

			VkSubpassDescription subpassDescription = {};
			subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpassDescription.colorAttachmentCount = subpassInfos[i].ColorAttachmentReferences.Count();
			subpassDescription.pColorAttachments = subpassInfos[i].ColorAttachmentReferences.Data();
			subpassDescription.pDepthStencilAttachment = subpassInfos[i].DepthStencilAttachmentReference.has_value() ? 
				&subpassInfos[i].DepthStencilAttachmentReference.value() : VK_NULL_HANDLE;
			subpassDescription.preserveAttachmentCount = subpassInfos[i].PreserveAttachments.Count();
			subpassDescription.pPreserveAttachments = subpassInfos[i].PreserveAttachments.Data();
			subpassDescription.inputAttachmentCount = 0; // TODO
			subpassDescription.pResolveAttachments = nullptr; // TODO

			subpassInfos[i].SubpassDescription = subpassDescription;
			subpasses.Add(subpassDescription);
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
		
		VkRenderPass renderPass;
		AssertVkResult(vkCreateRenderPass(_device->GetDevice(), &createInfo, nullptr, &renderPass));

		return renderPass;
	}
}