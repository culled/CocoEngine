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
		ImageVulkan(GraphicsDeviceVulkan* device, ImageDescription description, VkImage image, VkImageView nativeView, bool isManagedInternally = false);
		virtual ~ImageVulkan() override;

		virtual void* GetResource() const override { return GetImage(); }

		/// <summary>
		/// Gets the VkImage object
		/// </summary>
		/// <returns>The VkImage</returns>
		VkImage GetImage() const { return _image; }

		VkImageView GetNativeView() const { return _nativeView; }
	};
}