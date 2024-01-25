#include "Renderpch.h"
#include "VulkanFramebuffer.h"
#include "VulkanRenderPass.h"
#include "../VulkanGraphicsDevice.h"
#include "../VulkanImage.h"
#include <Coco/Core/Math/Math.h>
#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	VulkanFramebuffer::VulkanFramebuffer(uint64 id, VulkanGraphicsDevice& device) :
		CachedVulkanResource(id),
		_device(device),
		_framebuffer(nullptr),
		_size(SizeInt::Zero)
	{}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		DestroyFramebuffer();
	}

	uint64 VulkanFramebuffer::MakeKey(const VulkanRenderPass& renderPass, std::span<const VulkanImage*> attachmentImages)
	{
		uint64 imageHash = std::accumulate(attachmentImages.begin(), attachmentImages.end(),
			static_cast<uint64>(0),
			[](uint64 hash, const VulkanImage* image)
			{
				return Math::CombineHashes(hash, image->ID);
			});

		return Math::CombineHashes(imageHash, renderPass.ID);
	}

	bool VulkanFramebuffer::NeedsUpdate(std::span<const VulkanImage*> attachmentImages) const
	{
		const ImageDescription& desc = attachmentImages.front()->GetDescription();
		SizeInt imageSize(static_cast<int>(desc.Width), static_cast<int>(desc.Height));

		return _framebuffer == nullptr || _size != imageSize;
	}

	void VulkanFramebuffer::Update(const VulkanRenderPass& renderPass, std::span<const VulkanImage*> attachmentImages)
	{
		DestroyFramebuffer();
		CreateFramebuffer(renderPass, attachmentImages);
	}

	void VulkanFramebuffer::CreateFramebuffer(const VulkanRenderPass& renderPass, std::span<const VulkanImage*> attachmentImages)
	{
		const ImageDescription& desc = attachmentImages.front()->GetDescription();
		_size = SizeInt(static_cast<int>(desc.Width), static_cast<int>(desc.Height));

		VkFramebufferCreateInfo framebufferInfo{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
		framebufferInfo.renderPass = renderPass.GetRenderPass();
		framebufferInfo.width = desc.Width;
		framebufferInfo.height = desc.Height;
		framebufferInfo.layers = 1;

		std::vector<VkImageView> imageViews;
		imageViews.reserve(attachmentImages.size());

		std::transform(attachmentImages.begin(), attachmentImages.end(),
			std::back_inserter(imageViews),
			[](const VulkanImage* image) { return image->GetNativeView(); }
		);

		framebufferInfo.attachmentCount = static_cast<uint32_t>(imageViews.size());
		framebufferInfo.pAttachments = imageViews.data();

		AssertVkSuccess(vkCreateFramebuffer(_device.GetDevice(), &framebufferInfo, _device.GetAllocationCallbacks(), &_framebuffer));

		CocoTrace("Created VulkanFramebuffer {} data with size {}", ID, _size.ToString())
	}

	void VulkanFramebuffer::DestroyFramebuffer()
	{
		if (!_framebuffer)
			return;

		_device.GetQueue(VulkanQueueType::Graphics)->WaitForIdle();
		vkDestroyFramebuffer(_device.GetDevice(), _framebuffer, _device.GetAllocationCallbacks());
		_framebuffer = nullptr;
		_size = SizeInt::Zero;

		CocoTrace("Destroyed VulkanFramebuffer {} data", ID)
	}
}