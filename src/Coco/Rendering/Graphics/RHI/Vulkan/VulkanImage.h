#pragma once

#include "../../Image.h"
#include "../../GraphicsDeviceResource.h"
#include "VulkanBuffer.h"

#include "VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
    class VulkanGraphicsDevice;
    class VulkanCommandBuffer;

    /// @brief Vulkan implementation of an Image
    class VulkanImage : public Image, public GraphicsDeviceResource<VulkanGraphicsDevice>
    {
        friend class VulkanRenderContext;

    private:
        bool _isManagedExternally;
        VkImage _image;
        ImageDescription _description;
        VkImageLayout _currentLayout;
        VkImageView _nativeView;
        VkDeviceMemory _imageMemory;
        uint32 _memoryIndex;

    public:
        VulkanImage(const GraphicsDeviceResourceID& id, const ImageDescription& description);
        VulkanImage(const GraphicsDeviceResourceID& id, const ImageDescription& description, VkImage image);
        ~VulkanImage();

        ImageDescription GetDescription() const final { return _description; }
        uint64 GetSize() const final;
        void SetPixels(uint64 offset, const void* pixelData, uint64 pixelDataSize) final;

        /// @brief Gets the Vulkan image
        /// @return The Vulkan image
        VkImage GetImage() const { return _image; }

        /// @brief Gets the native view onto this image
        /// @return The native image view
        VkImageView GetNativeView() const { return _nativeView; }

        /// @brief Transitions this image to a new layout
        /// @param commandBuffer The command buffer to use
        /// @param to The layout to transition to
        void TransitionLayout(VulkanCommandBuffer& commandBuffer, VkImageLayout to);

    private:
        /// @brief Creates the image from the set description
        void CreateImage();

        /// @brief Creates the native image view
        void CreateNativeImageView();

        void CopyFromBuffer(VulkanCommandBuffer& commandBuffer, VulkanBuffer& source);
    };
}