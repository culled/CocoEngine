#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Rendering/Graphics/Image.h>
#include "CommandBufferVulkan.h"
#include "BufferVulkan.h"

#include "VulkanIncludes.h"

namespace Coco::Rendering
{
	class GraphicsDevice;
	class GraphicsDeviceVulkan;

	/// <summary>
	/// Vulkan-implementation of an Image
	/// </summary>
	class ImageVulkan final : public Image
	{
	private:
		bool _isManagedInternally;
		VkImage _image = nullptr;
		VkImageView _nativeView = nullptr;
		VkDeviceMemory _imageMemory = nullptr;
		GraphicsDeviceVulkan* _device;
		uint32_t _memoryIndex = 0;

	public:
		ImageVulkan(GraphicsDevice* device, ImageDescription description, VkImage image);
		ImageVulkan(GraphicsDevice* device, ImageDescription description);
		~ImageVulkan() final;

		void SetPixelData(const void* pixelData) final;

		/// <summary>
		/// Gets the VkImage object
		/// </summary>
		/// <returns>The VkImage</returns>
		VkImage GetImage() const noexcept { return _image; }

		/// <summary>
		/// Gets the native view onto the image
		/// </summary>
		/// <returns>The native view onto the image</returns>
		VkImageView GetNativeView() const noexcept { return _nativeView; }

		/// <summary>
		/// Copies image data from a buffer using a command buffer
		/// </summary>
		/// <param name="commandBuffer">The command buffer</param>
		/// <param name="buffer">The buffer to copy data from</param>
		void CopyFromBuffer(const CommandBufferVulkan* commandBuffer, BufferVulkan* buffer);

		/// <summary>
		/// Transitions this image between layouts
		/// </summary>
		/// <param name="commandBuffer">The command buffer</param>
		/// <param name="from">The layout to transition from</param>
		/// <param name="to">The layout to transition to</param>
		void TransitionLayout(const CommandBufferVulkan* commandBuffer, VkImageLayout from, VkImageLayout to);

	private:
		/// <summary>
		/// Creates an image from the current description
		/// </summary>
		void CreateImageFromDescription();

		/// <summary>
		/// Creates the native image view for the image
		/// </summary>
		void CreateNativeImageView();
	};
}