#pragma once

#include "../../../../Renderpch.h"
#include "CachedVulkanResource.h"
#include <Coco/Core/Types/Size.h>
#include "../VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
	class VulkanGraphicsDevice;
	class VulkanImage;
	class VulkanRenderPass;

	/// @brief A Vulkan framebuffer
	class VulkanFramebuffer : 
		public CachedVulkanResource
	{
	private:
		uint64 _version;
		VkFramebuffer _framebuffer;
		SizeInt _size;

	public:
		VulkanFramebuffer(const VulkanRenderPass& renderPass, std::span<const VulkanImage*> attachmentImages);
		~VulkanFramebuffer();

		/// @brief Creates a key unique to the provided values
		/// @param renderPass The render pass being used
		/// @param attachmentImages The attachment images
		/// @return A unique key
		static GraphicsDeviceResourceID MakeKey(const VulkanRenderPass& renderPass, std::span<const VulkanImage*> attachmentImages);

		/// @brief Gets this framebuffer's version
		/// @return The version
		uint64 GetVersion() const { return _version; }

		/// @brief Gets the Vulkan framebuffer
		/// @return The Vulkan framebuffer
		VkFramebuffer GetFramebuffer() const { return _framebuffer; }

		/// @brief Determines if this framebuffer needs to be updated
		/// @param renderPass The render pass
		/// @param size The size of the attachment images
		/// @return True if this framebuffer should be updated
		bool NeedsUpdate(const VulkanRenderPass& renderPass, const SizeInt& size) const;

		/// @brief Updates this framebuffer
		/// @param renderPass The render pass
		/// @param size The framebuffer size
		/// @param attachmentImages The attachment images
		void Update(const VulkanRenderPass& renderPass, const SizeInt& size, std::span<const VulkanImage*> attachmentImages);

	private:
		/// @brief Creates the framebuffer
		/// @param size The framebuffer size
		/// @param renderPass The render pass
		/// @param attachmentImages The attachment images
		void CreateFramebuffer(const SizeInt& size, const VulkanRenderPass& renderPass, std::span<const VulkanImage*> attachmentImages);

		/// @brief Destroys the framebuffer
		void DestroyFramebuffer();
	};
}