#include "Renderpch.h"
#include "VulkanFramebuffer.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanRenderContextCache.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	VulkanFramebuffer::VulkanFramebuffer(const SizeInt& size, VulkanRenderPass& renderPass, const std::vector<VulkanImage*>& attachmentImages) :
		_key(MakeKey(size, renderPass, attachmentImages)),
		_size(size),
		_renderPass(renderPass.GetRenderPass()),
		_attachmentImages(attachmentImages),
		_framebuffer(nullptr)
	{
		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = _renderPass;
		framebufferInfo.width = static_cast<uint32_t>(size.Width);
		framebufferInfo.height = static_cast<uint32_t>(size.Height);
		framebufferInfo.layers = 1;

		std::vector<VkImageView> imageViews;
		for (VulkanImage* image : attachmentImages)
		{
			imageViews.push_back(image->GetNativeView());
		}

		framebufferInfo.attachmentCount = static_cast<uint32_t>(imageViews.size());
		framebufferInfo.pAttachments = imageViews.data();

		AssertVkSuccess(vkCreateFramebuffer(_device->GetDevice(), &framebufferInfo, _device->GetAllocationCallbacks(), &_framebuffer));

		CocoTrace("Created VulkanFramebuffer")
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		if (_framebuffer)
		{
			_device->WaitForIdle();
			vkDestroyFramebuffer(_device->GetDevice(), _framebuffer, _device->GetAllocationCallbacks());
			_framebuffer = nullptr;
		}

		_attachmentImages.clear();

		CocoTrace("Destroyed VulkanFramebuffer")
	}

	VulkanFramebufferKey VulkanFramebuffer::MakeKey(const SizeInt& size, VulkanRenderPass& renderPass, const std::vector<VulkanImage*>& attachmentImages)
	{
		std::hash<VulkanImage*> imageHasher;
		uint64 imageHash = 0;

		for (VulkanImage* image : attachmentImages)
		{
			imageHash = Math::CombineHashes(imageHash, imageHasher(image));
		}

		return Math::CombineHashes(size.Width, size.Height, renderPass.GetKey(), imageHash);
	}

	void VulkanFramebuffer::Use()
	{
		_lastUsedTime = Engine::cGet()->GetMainLoop()->GetCurrentTick().UnscaledTime;
	}

	bool VulkanFramebuffer::IsStale() const
	{
		double currentTime = Engine::cGet()->GetMainLoop()->GetCurrentTick().UnscaledTime;
		return currentTime - _lastUsedTime > VulkanRenderContextCache::sPurgeThreshold;
	}
}