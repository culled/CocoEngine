#pragma once

#include "../../Image.h"
#include "../../GraphicsDeviceResource.h"

#include "VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
    class VulkanGraphicsDevice;
    class VulkanCommandBuffer;
    class VulkanBuffer;

    /// @brief Data for a Vulkan image
    struct VulkanImageData
    {
        /// @brief The image
        VkImage Image;

        /// @brief The image's memory
        VkDeviceMemory Memory;

        /// @brief The head index of the image memory
        uint32 HeapIndex;

        /// @brief The current layout of the image
        VkImageLayout CurrentLayout;

        VulkanImageData();
        VulkanImageData(VkImage image);
    };

    /// @brief Vulkan implementation of an Image
    class VulkanImage : 
        public Image, 
        public GraphicsDeviceResource<VulkanGraphicsDevice>
    {
        friend class VulkanRenderContext;

    private:
        ImageDescription _description;
        VulkanImageData _imageData;
        VkImageView _nativeView;
        VulkanImageData _hostImageData;

    public:
        VulkanImage(const GraphicsDeviceResourceID& id, const ImageDescription& description);
        VulkanImage(const GraphicsDeviceResourceID& id, const ImageDescription& description, VkImage image);
        ~VulkanImage();

        ImageDescription GetDescription() const final { return _description; }
        uint64 GetDataSize() const final;
        void SetPixels(uint64 offset, const void* pixelData, uint64 pixelDataSize) final;
        void ReadPixel(const Vector2Int& pixelCoords, void* outData, size_t dataSize) final;

        /// @brief Gets the Vulkan image
        /// @return The Vulkan image
        VkImage GetImage() const { return _imageData.Image; }

        /// @brief Gets the native view onto this image
        /// @return The native image view
        VkImageView GetNativeView() const { return _nativeView; }

        /// @brief Transitions this image to a new layout
        /// @param commandBuffer The command buffer to use
        /// @param to The layout to transition to
        void TransitionLayout(VulkanCommandBuffer& commandBuffer, VkImageLayout to);

    private:
        /// @brief Creates the image from the set description
        /// @param hostVisible If true, the image will be created so that it is host visible
        /// @param outImageData Will be filled with the created image
        void CreateImage(bool hostVisible, VulkanImageData& outImageData);

        /// @brief Destroys an image
        /// @param imageData The image to destroy
        void DestroyImage(VulkanImageData& imageData);

        /// @brief Creates the native image view
        void CreateNativeImageView();

        /// @brief Copies pixel data into this image from a buffer
        /// @param commandBuffer The command buffer
        /// @param source The source buffer
        void CopyFromBuffer(VulkanCommandBuffer& commandBuffer, VulkanBuffer& source);

        /// @brief Transitions an image to a new layout
        /// @param commandBuffer The command buffer to use
        /// @param to The layout to transition to
        /// @param imageData The image to transition
        void TransitionLayout(VulkanCommandBuffer& commandBuffer, VkImageLayout to, VulkanImageData& imageData);

        /// @brief Copies an image to the destination image
        /// @param src The source image
        /// @param srcRegion The region of the source image
        /// @param dstData The image to copy to
        /// @param dstEndingLayout The layout that the destination image will be transitioned to after the copy finishes
        void Copy(VulkanImage& src, const RectInt& srcRegion, VulkanImageData& dstData, VkImageLayout dstEndingLayout);

        /// @brief Generates mip maps from the base image
        /// @param commandBuffer The command buffer
        void GenerateMipMaps(VulkanCommandBuffer& commandBuffer);
    };
}