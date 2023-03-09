#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Rendering/Graphics/Image.h>

#include "VulkanIncludes.h"

namespace Coco::Rendering
{
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
		GraphicsDeviceVulkan* _device;

	public:
		ImageVulkan(GraphicsDevice* device, ImageDescription description, VkImage image, VkImageView nativeView, bool isManagedInternally = false);
		virtual ~ImageVulkan() override;

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
	};
}