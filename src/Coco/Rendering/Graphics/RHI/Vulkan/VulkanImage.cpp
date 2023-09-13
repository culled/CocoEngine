#include "Renderpch.h"
#include "VulkanImage.h"
#include "VulkanGraphicsDevice.h"

namespace Coco::Rendering::Vulkan
{
	VulkanImage::VulkanImage(const ImageDescription& description, VkImage image) :
		_image(image),
		_isManagedExternally(true),
		_description(description)
	{}

	VulkanImage::~VulkanImage()
	{
		if (_image && !_isManagedExternally)
		{
			vkDestroyImage(_device->GetDevice(), _image, _device->GetAllocationCallbacks());
		}

		_image = nullptr;
	}
}