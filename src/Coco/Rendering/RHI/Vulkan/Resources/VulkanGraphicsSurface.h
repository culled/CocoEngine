//
// Created by cullen on 3/22/26.
//

#ifndef COCOENGINE_VULKANGRAPHICSSURFACE_H
#define COCOENGINE_VULKANGRAPHICSSURFACE_H

#include "Coco/Rendering/Graphics/Resources/GraphicsSurface.h"
#include "../VulkanForwardDeclarations.h"

#include "Coco/Core/Memory/Refs.h"
#include "Coco/Core/Types/Array.h"

namespace Coco
{
    class VulkanGraphicsSemaphore;
    class VulkanGraphicsPlatform;
    class VulkanImage;

    struct VulkanSwapchainResource
    {
        ManagedRef<VulkanImage> SwapchainImage;
        Ref<VulkanGraphicsSemaphore> ImageAcquiredSempahore;
        ManagedRef<VulkanGraphicsSemaphore> RenderingCompleteSemaphore;

        VulkanSwapchainResource(ManagedRef<VulkanImage>&& swapchainImage, ManagedRef<VulkanGraphicsSemaphore>&& renderingCompleteSemaphore);
    };

    class VulkanGraphicsSurface : public GraphicsSurface
    {
    public:
        VulkanGraphicsSurface(uint64 id, VulkanGraphicsPlatform* platform, VkSurfaceKHR surface, const Sizei& framebufferSize);
        ~VulkanGraphicsSurface();

        void SetFramebufferSize(const Sizei& framebufferSize) override;
        Sizei GetFramebufferSize() const override { return _framebufferSize; }
        void SetVSyncMode(VSyncMode mode) override;
        VSyncMode GetVSyncMode() const override { return _vsyncMode; }
        void RebuildIfNeeded() override;

        bool AcquireImage(Ref<VulkanGraphicsSemaphore> imageAcquiredSemaphore, Ref<VulkanImage>& outImage);
        Ref<VulkanGraphicsSemaphore> GetRenderingCompleteSemaphore();
        void Present();

    private:
        VulkanGraphicsPlatform* _platform;
        VkSurfaceKHR _surface;
        Sizei _framebufferSize;
        VSyncMode _vsyncMode;
        VkSwapchainKHR _swapchain;
        Array<VulkanSwapchainResource> _swapchainResources;
        uint32 _acquiredSwapchainImageIndex;
        bool _isDirty;

    private:
        void MarkDirty();
        bool EnsureSwapChain();
        VkSurfaceFormatKHR PickSurfaceFormat() const;
        VkExtent2D PickSurfaceExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) const;
        uint32 PickBackbufferCount(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) const;
        bool RecreateSwapChain();

        void DestroySwapChain(VkSwapchainKHR& swapchain);
    };
} // Coco

#endif //COCOENGINE_VULKANGRAPHICSSURFACE_H