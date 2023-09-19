#include "Renderpch.h"
#include "VulkanRenderPass.h"
#include "../VulkanGraphicsDevice.h"
#include "../VulkanUtils.h"
#include "../VulkanGraphicsDeviceCache.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	VulkanSubpassInfo::VulkanSubpassInfo() :
		ColorAttachments{},
		ColorAttachmentReferences{},
		DepthStencilAttachment{},
		DepthStencilAttachmentReference{},
		PreserveAttachments{},
		SubpassDescription{}
	{}

	VulkanRenderPass::VulkanRenderPass(CompiledRenderPipeline& pipeline) : 
		GraphicsDeviceResource<VulkanGraphicsDevice>(MakeKey(pipeline)),
		_renderPass(nullptr),
		_subpassInfos(pipeline.RenderPasses.size()),
		_attachments(pipeline.InputAttachments),
		_multisamplingMode(MSAASamples::One)
	{
		Assert(pipeline.RenderPasses.size() != 0)

		std::vector<VkAttachmentDescription> attachments;

		// Create attachment descriptions for all attachments
		for (const AttachmentFormat& attachment : pipeline.InputAttachments)
		{
			VkImageLayout layout = ToAttachmentLayout(attachment.PixelFormat);

			if (attachment.SampleCount > _multisamplingMode)
				_multisamplingMode = attachment.SampleCount;

			VkAttachmentDescription description{};
			description.samples = ToVkSampleCountFlagBits(attachment.SampleCount);
			description.format = ToVkFormat(attachment.PixelFormat, ImageColorSpace::Linear);

			description.loadOp = attachment.ShouldPreserve ?
				VK_ATTACHMENT_LOAD_OP_LOAD : VK_ATTACHMENT_LOAD_OP_CLEAR;

			description.storeOp = attachment.ShouldPreserve ?
				VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE;

			description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

			description.initialLayout = attachment.ShouldPreserve ?
				layout : VK_IMAGE_LAYOUT_UNDEFINED;

			description.finalLayout = layout;

			attachments.push_back(description);
		}

		std::vector<VkSubpassDescription> subpasses(pipeline.RenderPasses.size());

		// Create subpasses for all pipeline renderpasses
		for (size_t i = 0; i < pipeline.RenderPasses.size(); i++)
		{
			VulkanSubpassInfo& subpass = _subpassInfos.at(i);

			// Start assuming no attachments are used
			for (uint32 ii = 0; ii < pipeline.InputAttachments.size(); ii++)
			{
				subpass.PreserveAttachments.emplace_back(ii);
			}

			const RenderPassBinding& binding = pipeline.RenderPasses.at(i);

			// Go through each of the pass's attachment mappings
			for (const auto& bindingPair : binding.PipelineToPassIndexMapping)
			{
				const uint8 pipelineAttachmentIndex = bindingPair.first;
				const uint8 passAttachmentIndex = bindingPair.second;
				const AttachmentFormat& pipelineAttachmentFormat = pipeline.InputAttachments.at(pipelineAttachmentIndex);

				VkAttachmentReference attachmentRef{};
				attachmentRef.attachment = pipelineAttachmentIndex;

				attachmentRef.layout = ToAttachmentLayout(pipelineAttachmentFormat.PixelFormat);

				if (attachmentRef.layout != VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && !subpass.DepthStencilAttachmentReference.has_value())
				{
					// Only 1 depth/stencil attachment
					subpass.DepthStencilAttachmentReference = attachmentRef;
					subpass.DepthStencilAttachment = pipelineAttachmentFormat;
				}
				else
				{
					subpass.ColorAttachmentReferences.push_back(attachmentRef);
					subpass.ColorAttachments.push_back(pipelineAttachmentFormat);
				}

				// Remove the attachment since it is used
				const auto it = std::find(subpass.PreserveAttachments.cbegin(), subpass.PreserveAttachments.cend(), pipelineAttachmentIndex);
				if (it != subpass.PreserveAttachments.cend())
					subpass.PreserveAttachments.erase(it);
			}

			VkSubpassDescription subpassDescription{};
			subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

			subpassDescription.colorAttachmentCount = static_cast<uint32_t>(subpass.ColorAttachmentReferences.size());
			subpassDescription.pColorAttachments = subpass.ColorAttachmentReferences.data();

			subpassDescription.pDepthStencilAttachment = subpass.DepthStencilAttachmentReference.has_value() ?
				&subpass.DepthStencilAttachmentReference.value() : VK_NULL_HANDLE;

			subpassDescription.preserveAttachmentCount = static_cast<uint32_t>(subpass.PreserveAttachments.size());
			subpassDescription.pPreserveAttachments = subpass.PreserveAttachments.data();

			subpassDescription.inputAttachmentCount = 0; // TODO: input attachments
			subpassDescription.pResolveAttachments = nullptr; // TODO: MSAA resolve attachments

			subpass.SubpassDescription = subpassDescription;
			subpasses.at(i) = subpassDescription;
		}

		std::vector<VkSubpassDependency> subpassDependencies(pipeline.RenderPasses.size() + 1);

		// Wait until the previous pipeline operation has completed before performing the vertex stage
		VkSubpassDependency& initialDependency = subpassDependencies.front();
		initialDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		initialDependency.dstSubpass = 0;
		initialDependency.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		initialDependency.dstStageMask = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
		initialDependency.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
		initialDependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		initialDependency.dependencyFlags = 0;

		for (uint32_t i = 0; i < static_cast<uint32_t>(pipeline.RenderPasses.size() - 1); i++)
		{
			// Wait until the previous subpass has written to the color attachments before performing the vertex stage on the next pass
			VkSubpassDependency& dependency = subpassDependencies.at(i);
			dependency.srcSubpass = i;
			dependency.dstSubpass = i + 1;
			dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.dstStageMask = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
			dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			dependency.dependencyFlags = 0;
		}

		// Wait until the last subpass finishes writing color attachments before ending the pipeline operation
		VkSubpassDependency& finalDependency = subpassDependencies.back();
		finalDependency.srcSubpass = static_cast<uint32_t>(pipeline.RenderPasses.size() - 1);
		finalDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
		finalDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		finalDependency.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		finalDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		finalDependency.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		finalDependency.dependencyFlags = 0;

		VkRenderPassCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		createInfo.pAttachments = attachments.data();
		createInfo.subpassCount = static_cast<uint32_t>(subpasses.size());
		createInfo.pSubpasses = subpasses.data();
		createInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());
		createInfo.pDependencies = subpassDependencies.data();

		AssertVkSuccess(vkCreateRenderPass(_device->GetDevice(), &createInfo, _device->GetAllocationCallbacks(), &_renderPass));

		CocoTrace("Created VulkanRenderPass")
	}

	VulkanRenderPass::~VulkanRenderPass()
	{
		if (_renderPass)
		{
			_device->WaitForIdle();
			vkDestroyRenderPass(_device->GetDevice(), _renderPass, _device->GetAllocationCallbacks());
			_renderPass = nullptr;
		}

		CocoTrace("Destroyed VulkanRenderPass")
	}

	GraphicsDeviceResourceID VulkanRenderPass::MakeKey(CompiledRenderPipeline& pipeline)
	{
		return Math::CombineHashes(pipeline.Version, pipeline.PipelineID);
	}

	const VulkanSubpassInfo& VulkanRenderPass::GetSubpassInfo(uint64 index) const
	{
		Assert(index < _subpassInfos.size())

		return _subpassInfos.at(index);
	}

	void VulkanRenderPass::Use()
	{
		_lastUsedTime = MainLoop::cGet()->GetCurrentTick().UnscaledTime;
	}

	bool VulkanRenderPass::IsStale() const
	{
		double currentTime = MainLoop::cGet()->GetCurrentTick().UnscaledTime;
		return currentTime - _lastUsedTime > VulkanGraphicsDeviceCache::sPurgeThreshold;
	}
}