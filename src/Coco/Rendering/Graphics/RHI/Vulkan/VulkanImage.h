#pragma once

#include "../../Image.h"
#include "../../GraphicsDeviceResource.h"

#include "VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
    class VulkanGraphicsDevice;

    /// @brief Vulkan implementation of an Image
    class VulkanImage : public Image, GraphicsDeviceResource<VulkanGraphicsDevice>
    {
    private:
        bool _isManagedExternally;
        VkImage _image;
        ImageDescription _description;

    public:
        VulkanImage(const ImageDescription& description, VkImage image);
        ~VulkanImage();

        ImageDescription GetDescription() const final { return _description; }

        /// @brief Gets the Vulkan image
        /// @return The Vulkan image
        VkImage GetImage() const { return _image; }
    };
}