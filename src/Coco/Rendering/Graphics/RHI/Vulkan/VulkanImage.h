#pragma once
#include "../../Image.h"

#include "VulkanIncludes.h"
#include <vk_mem_alloc.h>

namespace Coco::Rendering::Vulkan
{
    class VulkanGraphicsDevice;
    class VulkanCommandBuffer;
    class VulkanBuffer;
    class VulkanQueue;

    /// @brief Data for a Vulkan image
    struct VulkanImageInfo
    {
        /// @brief The image
        VkImage Image;
        VkImageView NativeView;

        VmaAllocation Memory;
        VmaAllocationInfo AllocInfo;

        /// @brief The current layout of the image
        VkImageLayout CurrentLayout;

        VulkanImageInfo();
        VulkanImageInfo(VkImage image);
    };

    class VulkanImage :
        public Image
    {
        friend class VulkanRenderContext;

    public:
        VulkanImage(const GraphicsResourceID& id, VulkanGraphicsDevice& device, const ImageDescription& description, VkImage image);
        VulkanImage(const GraphicsResourceID& id, VulkanGraphicsDevice& device, const ImageDescription& description);
        ~VulkanImage();

        // Inherited via Image
        const ImageDescription& GetDescription() const override { return _description; }
        uint64 GetDataSize() const override { return _imageInfo.AllocInfo.size; }
        void SetPixels(uint64 offset, const void* pixelData, uint64 pixelDataSize) override;

        void TransitionLayout(const VulkanCommandBuffer& commandBuffer, VulkanQueue& fromQueue, VulkanQueue& toQueue, VkImageLayout to);

        VkImage GetImage() const { return _imageInfo.Image; }
        VkImageView GetNativeView() const { return _imageInfo.NativeView; }

    private:
        VulkanGraphicsDevice& _device;
        ImageDescription _description;
        VulkanImageInfo _imageInfo;

    private:
        static void AdjustDescription(VulkanGraphicsDevice& device, ImageDescription& description);
        static void CreateImage(VulkanGraphicsDevice& device, const ImageDescription& description, bool enableHostVisible, VulkanImageInfo& outImageInfo);
        static void DestroyImage(VulkanGraphicsDevice& device, VulkanImageInfo& imageInfo);
        static void CreateNativeImageView(VulkanGraphicsDevice& device, const ImageDescription& description, VulkanImageInfo& imageInfo);
        static void DestroyNativeImageView(VulkanGraphicsDevice& device, VulkanImageInfo& imageInfo);     
        static void TransitionLayout(VulkanGraphicsDevice& device, const VulkanCommandBuffer& commandBuffer, VulkanQueue& fromQueue, VulkanQueue& toQueue, VkImageLayout to, const ImageDescription& description, VulkanImageInfo& imageData);

        /// @brief Copies pixel data into this image from a buffer
       /// @param commandBuffer The command buffer
       /// @param source The source buffer
        void CopyFromBuffer(const VulkanCommandBuffer& commandBuffer, const VulkanBuffer& source);

        /// @brief Generates mip maps from the base image. The image's end layout will be VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        /// @param commandBuffer The command buffer
        void GenerateMipMaps(const VulkanCommandBuffer& commandBuffer);
    };
}