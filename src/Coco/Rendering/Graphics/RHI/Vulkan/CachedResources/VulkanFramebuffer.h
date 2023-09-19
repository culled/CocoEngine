#pragma once

#include "../../../../Renderpch.h"
#include "../../../GraphicsDeviceResource.h"
#include "../VulkanIncludes.h"
#include <Coco/Core/Types/Size.h>
#include "../VulkanImage.h"
#include "VulkanRenderPass.h"

namespace Coco::Rendering::Vulkan
{
	class VulkanGraphicsDevice;

	/// @brief A Vulkan framebuffer
	class VulkanFramebuffer : public GraphicsDeviceResource<VulkanGraphicsDevice>
	{
	private:
		uint64 _version;
		VkFramebuffer _framebuffer;
		double _lastUsedTime;

	public:
		VulkanFramebuffer(const SizeInt& size, const VulkanRenderPass& renderPass, std::span<const VulkanImage*> attachmentImages);
		~VulkanFramebuffer();

		/// @brief Creates a key unique to the provided values
		/// @param size The size of the image being rendered
		/// @param renderPass The render pass being used
		/// @param attachmentImages Images being used as attachments
		/// @return A unique key
		static GraphicsDeviceResourceID MakeKey(const SizeInt& size, const VulkanRenderPass& renderPass, std::span<const VulkanImage*> attachmentImages);

		/// @brief Gets this framebuffer's version
		/// @return The version
		uint64 GetVersion() const { return _version; }

		/// @brief Gets the Vulkan framebuffer
		/// @return The Vulkan framebuffer
		VkFramebuffer GetFramebuffer() const { return _framebuffer; }

		/// @brief Determines if this framebuffer needs to be updated
		/// @param renderPass The render pass
		/// @return True if this framebuffer should be updated
		bool NeedsUpdate(const VulkanRenderPass& renderPass) const;

		/// @brief Updates this framebuffer
		/// @param size The framebuffer size
		/// @param renderPass The render pass
		/// @param attachmentImages The attachment images
		void Update(const SizeInt& size, const VulkanRenderPass& renderPass, std::span<const VulkanImage*> attachmentImages);

		/// @brief Marks this framebuffer as used
		void Use();

		/// @brief Determines if this framebuffer is stale and can be purged
		/// @return True if this framebuffer can be purged
		bool IsStale() const;

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