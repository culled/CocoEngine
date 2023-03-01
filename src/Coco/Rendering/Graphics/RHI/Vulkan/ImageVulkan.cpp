#include "ImageVulkan.h"

#include "GraphicsDeviceVulkan.h"

namespace Coco::Rendering
{
	ImageVulkan::ImageVulkan(GraphicsDeviceVulkan* device, ImageDescription description, VkImage image, VkImageView nativeView, bool isManagedInternally) : 
		Image(device, description),
		_device(device), _image(image), _nativeView(nativeView), _isManagedInternally(isManagedInternally)
	{
	}

	ImageVulkan::~ImageVulkan()
	{
		if(_nativeView != nullptr)
			vkDestroyImageView(_device->GetDevice(), _nativeView, nullptr);

		if(!_isManagedInternally && _image != nullptr)
			vkDestroyImage(_device->GetDevice(), _image, nullptr);

		_nativeView = nullptr;
		_image = nullptr;
	}
}