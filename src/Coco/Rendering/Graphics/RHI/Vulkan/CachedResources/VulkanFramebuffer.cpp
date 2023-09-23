#include "Renderpch.h"
#include "VulkanFramebuffer.h"
#include "../VulkanGraphicsDevice.h"
#include "VulkanRenderContextCache.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	VulkanFramebuffer::VulkanFramebuffer(const SizeInt& size, const VulkanRenderPass& renderPass, std::span<const VulkanImage*> attachmentImages) :
		GraphicsDeviceResource<VulkanGraphicsDevice>(MakeKey(size, renderPass, attachmentImages)),
		_version(0),
		_framebuffer(nullptr),
		_lastUsedTime(0.0)
	{}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		DestroyFramebuffer();
	}

	GraphicsDeviceResourceID VulkanFramebuffer::MakeKey(const SizeInt& size, const VulkanRenderPass& renderPass, std::span<const VulkanImage*> attachmentImages)
	{
		std::hash<const VulkanImage*> imageHasher;
		uint64 imageHash = 0;

		for (const VulkanImage* image : attachmentImages)
		{
			imageHash = Math::CombineHashes(imageHash, imageHasher(image));
		}

		return Math::CombineHashes(size.Width, size.Height, renderPass.ID, imageHash);
	}

	bool VulkanFramebuffer::NeedsUpdate(const VulkanRenderPass& renderPass) const
	{
		return _version != renderPass.GetVersion() || _framebuffer == nullptr;
	}

	void VulkanFramebuffer::Update(const SizeInt& size, const VulkanRenderPass& renderPass, std::span<const VulkanImage*> attachmentImages)
	{
		DestroyFramebuffer();
		CreateFramebuffer(size, renderPass, attachmentImages);

		_version = renderPass.GetVersion();
	}

	void VulkanFramebuffer::Use()
	{
		_lastUsedTime = MainLoop::cGet()->GetCurrentTick().UnscaledTime;
	}

	bool VulkanFramebuffer::IsStale() const
	{
		double currentTime = MainLoop::cGet()->GetCurrentTick().UnscaledTime;
		return currentTime - _lastUsedTime > VulkanRenderContextCache::sPurgeThreshold;
	}

	void VulkanFramebuffer::CreateFramebuffer(const SizeInt& size, const VulkanRenderPass& renderPass, std::span<const VulkanImage*> attachmentImages)
	{
		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass.GetRenderPass();
		framebufferInfo.width = static_cast<uint32_t>(size.Width);
		framebufferInfo.height = static_cast<uint32_t>(size.Height);
		framebufferInfo.layers = 1;

		std::vector<VkImageView> imageViews;
		for (const VulkanImage* image : attachmentImages)
		{
			imageViews.push_back(image->GetNativeView());
		}

		framebufferInfo.attachmentCount = static_cast<uint32_t>(imageViews.size());
		framebufferInfo.pAttachments = imageViews.data();

		AssertVkSuccess(vkCreateFramebuffer(_device.GetDevice(), &framebufferInfo, _device.GetAllocationCallbacks(), &_framebuffer));

		CocoTrace("Created VulkanFramebuffer")
	}

	void VulkanFramebuffer::DestroyFramebuffer()
	{
		if (!_framebuffer)
			return;

		_device.WaitForIdle();
		vkDestroyFramebuffer(_device.GetDevice(), _framebuffer, _device.GetAllocationCallbacks());
		_framebuffer = nullptr;

		CocoTrace("Destroyed VulkanFramebuffer")
	}
}