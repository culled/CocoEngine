#pragma once

#include <Coco/Rendering/Graphics/Resources/Image.h>
#include <Coco/Rendering/Graphics/Resources/GraphicsResource.h>

#include "../VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
	class GraphicsDeviceVulkan;
	class BufferVulkan;
	class CommandBufferVulkan;

	/// @brief Vulkan-implementation of an Image
	class ImageVulkan final : public GraphicsResource<GraphicsDeviceVulkan, Image>
	{
		friend class RenderContextVulkan;
		friend class ManagedRef<ImageVulkan>;

	private:
		bool _isManagedInternally;
		VkImage _image = nullptr;
		VkImageView _nativeView = nullptr;
		VkDeviceMemory _imageMemory = nullptr;
		uint32_t _memoryIndex = 0;
		VkImageLayout _currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	protected:
		ImageVulkan(const ResourceID& id, const string& name, const ImageDescription& description, VkImage image);
		ImageVulkan(const ResourceID& id, const string& name, const ImageDescription& description);

	public:
		~ImageVulkan() final;

		DefineResourceType(ImageVulkan)

		void SetPixelData(uint64_t offset, uint64_t size, const void* pixelData) final;

		/// @brief Gets the underlying VkImage
		/// @return The underlying Vulkan image
		VkImage GetImage() noexcept { return _image; }

		/// @brief Gets the native view onto the image
		/// @return The native view onto the image
		VkImageView GetNativeView() noexcept { return _nativeView; }

		/// @brief Copies image data from a buffer using a command buffer
		/// @param commandBuffer The command buffer
		/// @param buffer The buffer to copy data from
		void CopyFromBuffer(CommandBufferVulkan& commandBuffer, BufferVulkan& buffer);

		/// @brief Transitions this image between layouts
		/// @param commandBuffer The command buffer
		/// @param to The layout to transition to
		void TransitionLayout(CommandBufferVulkan& commandBuffer, VkImageLayout to);

	private:
		/// @brief Sets the current layout of this image
		/// @param layout The new layout
		void SetCurrentLayout(VkImageLayout layout) { _currentLayout = layout; }

		/// @brief Creates an image from the current description
		void CreateImageFromDescription();

		/// @brief Creates the native image view for the image
		void CreateNativeImageView();
	};
}