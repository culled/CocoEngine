#pragma once

#include "../../../Renderpch.h"
#include "../../GraphicsDeviceResource.h"
#include "VulkanIncludes.h"
#include <Coco/Core/Types/Size.h>
#include "VulkanImage.h"
#include "VulkanRenderPass.h"

namespace Coco::Rendering::Vulkan
{
	class VulkanGraphicsDevice;

	/// @brief A Vulkan framebuffer
	class VulkanFramebuffer : public GraphicsDeviceResource<VulkanGraphicsDevice>
	{
	private:
		SizeInt _size;
		std::vector<VulkanImage*> _attachmentImages;
		VkRenderPass _renderPass;
		VkFramebuffer _framebuffer;
		double _lastUsedTime;

	public:
		VulkanFramebuffer(const SizeInt& size, VulkanRenderPass& renderPass, const std::vector<VulkanImage*>& attachmentImages);
		~VulkanFramebuffer();

		/// @brief Creates a key unique to the provided values
		/// @param size The size of the image being rendered
		/// @param renderPass The render pass being used
		/// @param attachmentImages Images being used as attachments
		/// @return A unique key
		static GraphicsDeviceResourceID MakeKey(const SizeInt& size, VulkanRenderPass& renderPass, const std::vector<VulkanImage*>& attachmentImages);

		/// @brief Gets the Vulkan framebuffer
		/// @return The Vulkan framebuffer
		VkFramebuffer GetFramebuffer() const { return _framebuffer; }

		/// @brief Marks this framebuffer as used
		void Use();

		/// @brief Determines if this framebuffer is stale and can be purged
		/// @return True if this framebuffer can be purged
		bool IsStale() const;
	};
}