#pragma once
#include "../../GraphicsPresenter.h"
#include "VulkanGraphicsDevice.h"
#include "../../GraphicsDeviceResource.h"
#include "VulkanRenderContext.h"
#include "VulkanImage.h"

#include "VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
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

    /// @brief Vulkan implementation of a GraphicsPresenter
    class VulkanGraphicsPresenter : public GraphicsPresenter, public GraphicsDeviceResource<VulkanGraphicsDevice>
    {
    private:
        static const ImageUsageFlags _sBackbufferUsageFlags;

        bool _isSwapchainDirty;
        SizeInt _framebufferSize;
        VSyncMode _vSyncMode;
        ImageDescription _backbufferDescription;

        std::vector<ManagedRef<VulkanImage>> _backbuffers;
        std::vector<ManagedRef<VulkanRenderContext>> _renderContexts;
        std::optional<uint32> _currentBackbufferIndex;

        VkSwapchainKHR _swapchain;
        VkSurfaceKHR _surface;

    public:
        VulkanGraphicsPresenter(const GraphicsDeviceResourceID& id);
        ~VulkanGraphicsPresenter();

        void InitializeSurface(const GraphicsPresenterSurface& surface) final;
        bool SurfaceInitialized() const final { return _surface != nullptr; }

        bool PrepareForRender(Ref<RenderContext>& outContext, Ref<Image>& outBackbuffer) final;
        bool Present(Ref<GraphicsSemaphore> frameCompletedSemaphore) final;

        void SetVSync(VSyncMode mode) final;
        VSyncMode GetVSync() const final { return _vSyncMode; }

        void SetFramebufferSize(const SizeInt& size) final;
        SizeInt GetFramebufferSize() const final { return _framebufferSize; }

    private:
        /// @brief Recreates the swapchain if neccessary
        /// @return True if the swapchain is ready for rendering
        bool EnsureSwapchain();

        /// @brief Marks the swapchain as needing to be re-created
        void MarkDirty();

        /// @brief Creates/recreates the swapchain using the current settings
        /// @return True if the swapchain was created successfully
        bool RecreateSwapchain();

        /// @brief Destroys all objects created alongside a swapchain
        void DestroySwapchainObjects();

        Ref<VulkanRenderContext> GetReadyRenderContext();
    };
}