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
	class ImageVulkan : public Image
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
		virtual ~ImageVulkan() override;

		virtual void SetPixelData(const void* pixelData) override;

		/// <summary>
		/// Gets the VkImage object
		/// </summary>
		/// <returns>The VkImage</returns>
		VkImage GetImage() const { return _image; }

		/// <summary>
		/// Gets the native view onto the image
		/// </summary>
		/// <returns>The native view onto the image</returns>
		VkImageView GetNativeView() const { return _nativeView; }

		void CopyFromBuffer(CommandBufferVulkan* commandBuffer, BufferVulkan* buffer);
		void TransitionLayout(CommandBufferVulkan* commandBuffer, VkImageLayout from, VkImageLayout to);

	private:
		void CreateImageFromDescription();
		void CreateNativeImageView();
	};
}