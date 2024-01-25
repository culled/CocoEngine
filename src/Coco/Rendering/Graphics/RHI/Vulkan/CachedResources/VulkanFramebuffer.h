#pragma once
#include "CachedVulkanResource.h"
#include <Coco/Core/Types/Size.h>

#include "../VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
    class VulkanGraphicsDevice;
    class VulkanRenderPass;
    class VulkanImage;

    class VulkanFramebuffer :
        public CachedVulkanResource
    {
    public:
        VulkanFramebuffer(uint64 id, VulkanGraphicsDevice& device);
        ~VulkanFramebuffer();

        static uint64 MakeKey(const VulkanRenderPass& renderPass, std::span<const VulkanImage*> attachmentImages);

        bool NeedsUpdate(std::span<const VulkanImage*> attachmentImages) const;
        void Update(const VulkanRenderPass& renderPass, std::span<const VulkanImage*> attachmentImages);

        VkFramebuffer GetFramebuffer() const { return _framebuffer; }

    private:
        VulkanGraphicsDevice& _device;
        VkFramebuffer _framebuffer;
        SizeInt _size;

    private:
        void CreateFramebuffer(const VulkanRenderPass& renderPass, std::span<const VulkanImage*> attachmentImages);
        void DestroyFramebuffer();
    };
}