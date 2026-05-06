//
// Created by cullen on 3/22/26.
//

#ifndef COCOENGINE_VULKANIMAGE_H
#define COCOENGINE_VULKANIMAGE_H
#include "Coco/Rendering/Graphics/Resources/Image.h"
#include "Coco/Rendering/RHI/Vulkan/VulkanIncludes.h"
#include <vk_mem_alloc.h>

namespace Coco
{
    class VulkanGraphicsPlatform;

    /// @brief Data for a Vulkan image
    struct VulkanImageInfo
    {
        /// @brief The image
        VkImage Image;

        /// @brief The native view on the image
        VkImageView NativeView;

        VkImageView DepthView;

        /// @brief Memory for the image
        VmaAllocation Memory;

        /// @brief Memory allocation info for the image
        VmaAllocationInfo AllocInfo;

        /// @brief The current layout of the image
        VkImageLayout CurrentLayout;

        VulkanImageInfo();
    };

    class VulkanImage : public Image
    {
    public:
        VulkanImage(uint64 id, VulkanGraphicsPlatform* platform, const ImageDescription& description);
        VulkanImage(uint64 id, VulkanGraphicsPlatform* platform, const ImageDescription& description, VkImage image);
        ~VulkanImage();

        void SetPixels(const void* pixelData, uint64 pixelDataSize) override;

        //void TransitionLayout(VkCommandBuffer commandBuffer, VkImageLayout newLayout, VulkanQueue& targetQueue);
        void TransitionLayout(VkCommandBuffer commandBuffer, VkImageLayout newLayout);

        VkImageView GetNativeView() const { return _imageInfo.NativeView; }

    private:
        VulkanGraphicsPlatform* _platform;
        VulkanImageInfo _imageInfo;
        //VulkanQueue* _lastOperationQueue;

    private:
        void CreateImage(const ImageDescription& description, VulkanImageInfo& outImageInfo);
        void CreateNativeView(const ImageDescription& description, VulkanImageInfo& outImageInfo);
        //void CreateDepthView(const ImageDescription& description, VulkanImageInfo& outImageInfo);

        void GenerateMipMaps(VkCommandBuffer commandBuffer);

        void DestroyImage(VulkanImageInfo& imageInfo);
    };
} // Coco

#endif //COCOENGINE_VULKANIMAGE_H