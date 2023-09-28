#include "Renderpch.h"
#include "VulkanRenderPass.h"
#include "../../../../Pipeline/CompiledRenderPipeline.h"
#include "../VulkanGraphicsDevice.h"
#include "../VulkanUtils.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	VulkanSubpassInfo::VulkanSubpassInfo() :
		ColorAttachments{},
		ColorAttachmentReferences{},
		DepthStencilAttachment{},
		DepthStencilAttachmentReference{},
		ResolveAttachmentReferences{},
		PreserveAttachments{},
		SubpassDescription{}
	{}

	VulkanRenderPass::VulkanRenderPass(const CompiledRenderPipeline& pipeline, MSAASamples samples) :
		CachedVulkanResource(MakeKey(pipeline, samples)),
		_version(0),
		_renderPass(nullptr),
		_subpassInfos{},
		_samples(GetAdjustedSamples(pipeline, samples))
	{}

	VulkanRenderPass::~VulkanRenderPass()
	{
		DestroyRenderPass();
	}

	GraphicsDeviceResourceID VulkanRenderPass::MakeKey(const CompiledRenderPipeline& pipeline, MSAASamples samples)
	{
		return Math::CombineHashes(pipeline.PipelineID, static_cast<uint64>(samples));
	}

	const VulkanSubpassInfo& VulkanRenderPass::GetSubpassInfo(uint64 index) const
	{
		Assert(index < _subpassInfos.size())

		return _subpassInfos.at(index);
	}

	bool VulkanRenderPass::NeedsUpdate(const CompiledRenderPipeline& pipeline, MSAASamples samples) const
	{
		return _renderPass == nullptr || pipeline.Version != _version || _samples != GetAdjustedSamples(pipeline, samples);
	}

	void VulkanRenderPass::Update(const CompiledRenderPipeline& pipeline, MSAASamples samples, std::span<const uint8> resolveAttachmentIndices)
	{
		DestroyRenderPass();
		CreateRenderPass(pipeline, samples, resolveAttachmentIndices);

		_version = pipeline.Version;
	}

	MSAASamples VulkanRenderPass::GetAdjustedSamples(const CompiledRenderPipeline& pipeline, MSAASamples samples) const
	{
		const GraphicsDeviceFeatures& features = _device.GetFeatures();
		return pipeline.SupportsMSAA ? static_cast<MSAASamples>(Math::Min(samples, features.MaximumMSAASamples)) : MSAASamples::One;
	}

	void VulkanRenderPass::CreateRenderPass(const CompiledRenderPipeline& pipeline, MSAASamples samples, std::span<const uint8> resolveAttachmentIndices)
	{
		_subpassInfos.resize(pipeline.RenderPasses.size());
		_samples = GetAdjustedSamples(pipeline, samples);

		Assert(pipeline.RenderPasses.size() != 0)

		std::vector<VkAttachmentDescription> attachments;

		// Create attachment descriptions for all primary attachments
		for (const AttachmentFormat& attachment : pipeline.InputAttachments)
		{
			VkImageLayout layout = ToAttachmentLayout(attachment.PixelFormat);

			VkAttachmentDescription description{};
			description.samples = ToVkSampleCountFlagBits(_samples);
			description.format = ToVkFormat(attachment.PixelFormat, attachment.ColorSpace);

			description.loadOp = attachment.ShouldClear ?
				VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;

			description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

			description.stencilLoadOp = attachment.ShouldClear ? 
				VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
			description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

			description.initialLayout = attachment.ShouldClear ?
				VK_IMAGE_LAYOUT_UNDEFINED : layout;

			description.finalLayout = layout;

			attachments.push_back(description);
		}

		// Add attachment refs for any resolve attachments
		for (const uint8 i : resolveAttachmentIndices)
		{
			const AttachmentFormat& attachment = pipeline.InputAttachments.at(i);

			VkAttachmentDescription description = attachments.at(i);
			description.samples = VK_SAMPLE_COUNT_1_BIT;

			attachments.push_back(description);
		}

		std::vector<VkSubpassDescription> subpasses(pipeline.RenderPasses.size());
		uint64 totalAttachmentCount = pipeline.InputAttachments.size() + resolveAttachmentIndices.size();

		// Create subpasses for all pipeline renderpasses
		for (size_t i = 0; i < pipeline.RenderPasses.size(); i++)
		{
			VulkanSubpassInfo& subpass = _subpassInfos.at(i);

			// Start assuming no attachments are used
			for (uint32 ii = 0; ii < totalAttachmentCount; ii++)
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

				// Create an unused resolve attachment reference
				VkAttachmentReference resolveAttachmentRef{};
				resolveAttachmentRef.attachment = VK_ATTACHMENT_UNUSED;
				resolveAttachmentRef.layout = attachmentRef.layout;
				subpass.ResolveAttachmentReferences.push_back(resolveAttachmentRef);

				// Add a resolve attachment for this attachment if needed
				for (uint8 rI = 0; rI < resolveAttachmentIndices.size(); rI++)
				{
					if (resolveAttachmentIndices[rI] != pipelineAttachmentIndex)
						continue;

					uint32 attachmentIndex = static_cast<uint32>(pipeline.InputAttachments.size()) + rI;
					subpass.ResolveAttachmentReferences.at(rI).attachment = attachmentIndex;

					// Remove the attachment since it is used
					const auto it = std::find(subpass.PreserveAttachments.cbegin(), subpass.PreserveAttachments.cend(), attachmentIndex);
					if (it != subpass.PreserveAttachments.cend())
						subpass.PreserveAttachments.erase(it);
				}
			}

			VkSubpassDescription subpassDescription{};
			subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

			subpassDescription.colorAttachmentCount = static_cast<uint32_t>(subpass.ColorAttachmentReferences.size());
			subpassDescription.pColorAttachments = subpass.ColorAttachmentReferences.size() > 0 ? 
				subpass.ColorAttachmentReferences.data() : VK_NULL_HANDLE;

			subpassDescription.pDepthStencilAttachment = subpass.DepthStencilAttachmentReference.has_value() ?
				&subpass.DepthStencilAttachmentReference.value() : VK_NULL_HANDLE;

			subpassDescription.preserveAttachmentCount = static_cast<uint32_t>(subpass.PreserveAttachments.size());
			subpassDescription.pPreserveAttachments = subpass.PreserveAttachments.data();

			subpassDescription.pResolveAttachments = subpass.ResolveAttachmentReferences.data();

			subpassDescription.inputAttachmentCount = 0; // TODO: input attachments

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

		for (uint32_t i = 0; i < static_cast<uint32_t>(subpasses.size() - 1); i++)
		{
			// Wait until the previous subpass has written to the color attachments before performing the vertex stage on the next pass
			VkSubpassDependency& dependency = subpassDependencies.at(i + 1);
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
		finalDependency.srcSubpass = static_cast<uint32_t>(subpasses.size() - 1);
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

		AssertVkSuccess(vkCreateRenderPass(_device.GetDevice(), &createInfo, _device.GetAllocationCallbacks(), &_renderPass));

		CocoTrace("Created VulkanRenderPass")
	}

	void VulkanRenderPass::DestroyRenderPass()
	{
		if (!_renderPass)
			return;

		_device.WaitForIdle();

		vkDestroyRenderPass(_device.GetDevice(), _renderPass, _device.GetAllocationCallbacks());
		_renderPass = nullptr;

		_subpassInfos.clear();

		CocoTrace("Destroyed VulkanRenderPass")
	}
}