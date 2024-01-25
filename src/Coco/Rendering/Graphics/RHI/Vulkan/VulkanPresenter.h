#pragma once
#include <Coco/Core/Events/Event.h>
#include "../../Presenter.h"
#include "../../ImageTypes.h"
#include "VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
    class VulkanGraphicsDevice;
    class VulkanGraphicsSemaphore;
    class VulkanImage;

    struct VulkanPresenterSurface :
        public PresenterSurface
    {
        VkSurfaceKHR Surface;
    };

    /// @brief Support details for a swapchain
    struct SwapchainSupportDetails
    {
        /// @brief The surface capabilities
        VkSurfaceCapabilitiesKHR SurfaceCapabilities;

        /// @brief A list of supported surface formats
        std::vector<VkSurfaceFormatKHR> SurfaceFormats;

        /// @brief A list of supported present modes
        std::vector<VkPresentModeKHR> PresentModes;

        SwapchainSupportDetails();
    };

    class VulkanPresenter :
        public Presenter
    {
    public:
        VulkanPresenter(const GraphicsResourceID& id, VulkanGraphicsDevice& device);
        ~VulkanPresenter();

        // Inherited via Presenter
        bool HasSurface() const override { return _surface != nullptr; }
        void SetSurface(UniqueRef<PresenterSurface>&& surface) override;
        void SetFramebufferSize(const SizeInt& size) override;
        const SizeInt& GetFramebufferSize() override { return _framebufferSize; }
        void SetVSyncMode(VSyncMode mode) override;
        VSyncMode GetVSyncMode() const override { return _vSyncMode; }

        bool HasAcquiredImage() const { return _currentBackbufferIndex.has_value(); }
        bool AcquireImage(Ref<VulkanGraphicsSemaphore> imageAcquiredSemaphore, Ref<VulkanImage>& outImage);
        Ref<VulkanImage> GetAcquiredImage() const;
        bool Present(Ref<VulkanGraphicsSemaphore> waitSemaphore);

    private:
        VulkanGraphicsDevice& _device;

        VSyncMode _vSyncMode;
        SizeInt _framebufferSize;

        VkSwapchainKHR _swapchain;
        VkSurfaceKHR _surface;

        ImageDescription _backbufferDescription;
        std::vector<ManagedRef<VulkanImage>> _backbuffers;
        std::optional<uint32> _currentBackbufferIndex;

        bool _isSwapchainDirty;

        EventHandler<uint32> _handleFramesInFlightChanged;

    private:
        static SwapchainSupportDetails GetSwapchainSupportDetails(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);
        static VkPresentModeKHR PickPresentMode(VSyncMode preferredVSyncMode, const SwapchainSupportDetails& supportDetails);
        static VkSurfaceFormatKHR PickSurfaceFormat(const SwapchainSupportDetails& supportDetails);
        static VkExtent2D PickBackbufferExtent(const SizeInt& preferredSize, const SwapchainSupportDetails& supportDetails);
        static uint32 PickBackbufferCount(uint32 preferredCount, const SwapchainSupportDetails& supportDetails);

        /// @brief Recreates the swapchain if neccessary
        /// @return True if the swapchain is ready for rendering
        bool EnsureSwapchain();

        /// @brief Creates/recreates the swapchain using the current settings
        /// @return True if the swapchain was created successfully
        bool RecreateSwapchain();

        /// @brief Destroys all objects created alongside a swapchain
        void DestroySwapchainObjects();

        /// @brief Marks the swapchain as needing to be re-created
        void MarkDirty();

        /// @brief Handler for when the RenderService's frames in flight change
        /// @param framesInFlight The new frames in flight count
        /// @return If the event should be handled
        bool HandleFramesInFlightChanged(uint32 framesInFlight);
    };
}