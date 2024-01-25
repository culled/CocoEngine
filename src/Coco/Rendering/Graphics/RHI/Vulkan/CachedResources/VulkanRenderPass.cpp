#include "Renderpch.h"
#include "VulkanRenderPass.h"
#include "../VulkanGraphicsDevice.h"
#include "../../../../Pipeline/CompiledRenderPipeline.h"
#include "../VulkanUtils.h"

#include <Coco/Core/Math/Math.h>
#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	VulkanRenderPass::VulkanRenderPass(uint64 id, VulkanGraphicsDevice& device) :
		CachedVulkanResource(id),
		_device(device),
		_pipelineVersion(0),
		_renderPass(nullptr),
		_sampleCount(MSAASamples::One)
	{}

	VulkanRenderPass::~VulkanRenderPass()
	{
		DestroyRenderPass();
	}

	uint64 VulkanRenderPass::MakeKey(const CompiledRenderPipeline& pipeline, MSAASamples samples)
	{
		return Math::CombineHashes(static_cast<uint64>(samples), pipeline.PipelineID);
	}

	bool VulkanRenderPass::NeedsUpdate(const CompiledRenderPipeline& pipeline, MSAASamples samples)
	{
		return _renderPass == nullptr || samples != _sampleCount || pipeline.PipelineVersion != _pipelineVersion;
	}

	void VulkanRenderPass::Update(const CompiledRenderPipeline& pipeline, MSAASamples samples, std::span<const uint32> resolveAttachmentIndices)
	{
		DestroyRenderPass();
		CreateRenderPass(pipeline, samples, resolveAttachmentIndices);
	}

	MSAASamples VulkanRenderPass::GetAdjustedSampleCount(MSAASamples samples) const
	{
		return static_cast<MSAASamples>(
			Math::Min(
				static_cast<int>(_device.GetFeatures().MaximumMSAASamples),
				static_cast<int>(samples)
			)
		);
	}

	void VulkanRenderPass::CreateRenderPass(const CompiledRenderPipeline& pipeline, MSAASamples samples, std::span<const uint32> resolveAttachmentIndices)
	{
		std::vector<VkAttachmentDescription> attachmentDescriptions(pipeline.Attachments.size());
		_sampleCount = GetAdjustedSampleCount(samples);
		_pipelineVersion = pipeline.PipelineVersion;

		// Create attachment descriptions for all pipeline attachments
		for (uint64 i = 0; i < pipeline.Attachments.size(); i++)
		{
			const RenderPassAttachment& attachment = pipeline.Attachments.at(i);
			VkAttachmentDescription& attachmentDescription = attachmentDescriptions.at(i);

			VkImageLayout layout = ToVkImageLayout(attachment.PixelFormat);
			attachmentDescription.samples = ToVkSampleCountFlagBits(_sampleCount);
			attachmentDescription.format = ToVkFormat(attachment.PixelFormat, attachment.ColorSpace);

			bool clear = (attachment.OptionsFlags & AttachmentOptionsFlags::Clear) == AttachmentOptionsFlags::Clear;
			bool preserve = (attachment.OptionsFlags & AttachmentOptionsFlags::Preserve) == AttachmentOptionsFlags::Preserve;

			attachmentDescription.loadOp = clear ?
				VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;

			// Always store because a subsequent pipeline could use this attachment
			attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

			attachmentDescription.stencilLoadOp = attachmentDescription.loadOp;
			attachmentDescription.stencilStoreOp = attachmentDescription.storeOp;

			attachmentDescription.initialLayout = (preserve || !clear) ?
				layout : VK_IMAGE_LAYOUT_UNDEFINED;

			attachmentDescription.finalLayout = layout;
		}

		// Add attachment refs for any resolve attachments
		for (const uint32& i : resolveAttachmentIndices)
		{
			const VkAttachmentDescription& originalDescription = attachmentDescriptions.at(i);
			VkAttachmentDescription& attachmentDescription = attachmentDescriptions.emplace_back(originalDescription);
			attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
		}

		std::vector<VulkanSubPassInfo> subpasses(pipeline.RenderPasses.size());

		for (uint32 i = 0; i < pipeline.RenderPasses.size(); i++)
		{
			VulkanSubPassInfo& subpassInfo = subpasses.at(i);

			// Start assuming no attachments are used
			for (uint32 ii = 0; ii < attachmentDescriptions.size(); ii++)
			{
				subpassInfo.PreserveAttachmentIndices.emplace_back(ii);
			}

			const RenderPassPipelineBinding& binding = pipeline.RenderPasses.at(i);

			// Go through each of the pass's attachment mappings
			for (const auto& bindingPair : binding.PipelineToPassAttachmentMapping)
			{
				const uint32 pipelineAttachmentIndex = bindingPair.first;
				const uint32 passAttachmentIndex = bindingPair.second;
				const RenderPassAttachment& pipelineAttachment = pipeline.Attachments.at(pipelineAttachmentIndex);

				// Remove the attachment from the preserved list since it is used
				std::erase(subpassInfo.PreserveAttachmentIndices, pipelineAttachmentIndex);

				VkAttachmentReference attachmentRef{};
				attachmentRef.attachment = pipelineAttachmentIndex;
				attachmentRef.layout = ToVkImageLayout(pipelineAttachment.PixelFormat);

				if (attachmentRef.layout != VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && !subpassInfo.DepthStencilAttachmentReference.has_value())
				{
					// Only 1 depth/stencil attachment
					subpassInfo.DepthStencilAttachmentReference = attachmentRef;
				}
				else
				{
					subpassInfo.ColorAttachmentReferences.push_back(attachmentRef);
				}

				// Create an unused resolve attachment reference
				VkAttachmentReference& resolveAttachmentRef = subpassInfo.ResolveAttachmentReferences.emplace_back();
				resolveAttachmentRef.attachment = VK_ATTACHMENT_UNUSED;
				resolveAttachmentRef.layout = attachmentRef.layout;

				auto it = std::find(resolveAttachmentIndices.begin(), resolveAttachmentIndices.end(), pipelineAttachmentIndex);
				if (it != resolveAttachmentIndices.end())
				{
					uint32 resolveAttachmentIndex = static_cast<uint32>(pipeline.Attachments.size() + std::distance(resolveAttachmentIndices.begin(), it));
					resolveAttachmentRef.attachment = resolveAttachmentIndex;

					// Remove the attachment from the preserved list since it is used
					std::erase(subpassInfo.PreserveAttachmentIndices, resolveAttachmentIndex);
				}
			}

			VkSubpassDescription& subpassDesc = subpassInfo.Description;
			subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

			subpassDesc.colorAttachmentCount = static_cast<uint32>(subpassInfo.ColorAttachmentReferences.size());
			subpassDesc.pColorAttachments = subpassInfo.ColorAttachmentReferences.data();

			subpassDesc.pDepthStencilAttachment = subpassInfo.DepthStencilAttachmentReference.has_value() ?
				&subpassInfo.DepthStencilAttachmentReference.value() : VK_NULL_HANDLE;

			subpassDesc.pResolveAttachments = subpassInfo.ResolveAttachmentReferences.data();

			subpassDesc.inputAttachmentCount = 0; // TODO: input attachments

			subpassDesc.preserveAttachmentCount = static_cast<uint32>(subpassInfo.PreserveAttachmentIndices.size());
			subpassDesc.pPreserveAttachments = subpassInfo.PreserveAttachmentIndices.data();
		}

		std::vector<VkSubpassDependency> subpassDependencies;
		subpassDependencies.reserve(pipeline.RenderPasses.size() + 1);

		// Add initial dependencies to ensure the automatic transitions happen after image clearing has been written
		VkSubpassDependency& initialDependency = subpassDependencies.emplace_back();
		// Store op is always performed in late tests for depth/stencil attachments, color output for color attachments
		initialDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		initialDependency.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		initialDependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		// Load op is always performed in early tests for depth/stencil attachments, color output for color attachments
		initialDependency.dstSubpass = 0;
		initialDependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		initialDependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
			VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | 
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | 
			VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;

		initialDependency.dependencyFlags = 0;

		if (subpasses.size() > 0)
		{
			for (uint64 i = 0; i < subpasses.size() - 1; i++)
			{
				// Wait until the previous subpass has written to the color attachments before performing the vertex stage on the next pass
				VkSubpassDependency& dependency = subpassDependencies.emplace_back();
				dependency.srcSubpass = static_cast<uint32_t>(i);
				dependency.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

				dependency.dstSubpass = static_cast<uint32_t>(i + 1);
				dependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
					VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
					VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;

				dependency.dependencyFlags = 0;
			}

			// Wait until the last subpass finishes writing color attachments before ending the pipeline operation
			VkSubpassDependency& finalDependency = subpassDependencies.emplace_back();
			finalDependency.srcSubpass = static_cast<uint32_t>(subpasses.size() - 1);
			finalDependency.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			finalDependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			finalDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
			//finalDependency.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			//finalDependency.dstAccessMask = 0;
			finalDependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			finalDependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
				VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
				VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;

			finalDependency.dependencyFlags = 0;
		}
		else
		{
			initialDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
			initialDependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			initialDependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
				VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
				VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
		}

		VkRenderPassCreateInfo createInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
		createInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
		createInfo.pAttachments = attachmentDescriptions.data();

		std::vector<VkSubpassDescription> subpassDescriptions;
		std::transform(subpasses.begin(), subpasses.end(),
			std::back_inserter(subpassDescriptions),
			[](const VulkanSubPassInfo& info)
			{
				return info.Description;
			});

		createInfo.subpassCount = static_cast<uint32_t>(subpassDescriptions.size());
		createInfo.pSubpasses = subpassDescriptions.data();

		createInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());
		createInfo.pDependencies = subpassDependencies.data();

		AssertVkSuccess(vkCreateRenderPass(_device.GetDevice(), &createInfo, _device.GetAllocationCallbacks(), &_renderPass));

		CocoTrace("Created VulkanRenderPass {} data for pipeline {}", ID, pipeline.PipelineID.ToString())
	}

	void VulkanRenderPass::DestroyRenderPass()
	{
		if (!_renderPass)
			return;

		_device.WaitForIdle();

		vkDestroyRenderPass(_device.GetDevice(), _renderPass, _device.GetAllocationCallbacks());
		_renderPass = nullptr;
		_sampleCount = MSAASamples::One;
		_pipelineVersion = 0;

		CocoTrace("Destroyed VulkanRenderPass {} data", ID)
	}
}