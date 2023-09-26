#include "Renderpch.h"
#include "VulkanFramebuffer.h"
#include "../VulkanGraphicsDevice.h"
#include "VulkanRenderContextCache.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	VulkanFramebuffer::VulkanFramebuffer(const VulkanRenderPass& renderPass, std::span<const VulkanImage*> attachmentImages) :
		CachedVulkanResource(MakeKey(renderPass, attachmentImages)),
		_size(SizeInt::Zero),
		_attachmentImages(),
		_version(0),
		_framebuffer(nullptr)
	{}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		DestroyFramebuffer();
	}

	GraphicsDeviceResourceID VulkanFramebuffer::MakeKey(const VulkanRenderPass& renderPass, std::span<const VulkanImage*> attachmentImages)
	{
		static const std::hash<const VulkanImage*> imageHasher;

		uint64 imageHash = 0;
		for (size_t i = 0; i < attachmentImages.size(); i++)
			imageHash = Math::CombineHashes(imageHash, imageHasher(attachmentImages[i]));

		return Math::CombineHashes(renderPass.ID, imageHash);
	}

	bool VulkanFramebuffer::NeedsUpdate(const VulkanRenderPass& renderPass, const SizeInt& size) const
	{
		return _attachmentImages.size() == 0 ||
			_framebuffer == nullptr ||
			_version != renderPass.GetVersion() ||
			size != _size;
	}

	void VulkanFramebuffer::Update(const VulkanRenderPass& renderPass, const SizeInt& size, std::span<const VulkanImage*> attachmentImages)
	{
		DestroyFramebuffer();
		CreateFramebuffer(size, renderPass, attachmentImages);

		_version = renderPass.GetVersion();
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

		_size = size;
		_attachmentImages = std::vector<const VulkanImage*>(attachmentImages.begin(), attachmentImages.end());

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