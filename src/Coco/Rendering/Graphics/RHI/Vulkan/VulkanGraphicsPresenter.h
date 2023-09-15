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
        uint8 _currentContextIndex;
        ImageDescription _backbufferDescription;

        std::vector<UniqueRef<VulkanRenderContext>> _renderContexts;
        std::vector<ManagedRef<VulkanImage>> _backbuffers;
        std::vector<uint32> _acquiredBackbufferIndices;

        VkSwapchainKHR _swapchain;
        VkSurfaceKHR _surface;

    public:
        VulkanGraphicsPresenter(const GraphicsDeviceResourceID& id);
        ~VulkanGraphicsPresenter();

        void InitializeSurface(const SharedRef<GraphicsPresenterSurface>& surface) final;
        bool SurfaceInitialized() const final { return _surface != nullptr; }

        bool PrepareForRender(RenderContext*& outContext, Ref<Image>& outBackbuffer) final;
        bool Present(RenderContext& context) final;

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

        /// @brief Creates a RenderContext for each backbuffer image
        /// @return True if the render contexts were created successfully
        bool CreateRenderContexts();

        /// @brief Destroys all RenderContexts
        void DestroyRenderContexts();

        /// @brief Destroys all objects created alongside a swapchain
        void DestroySwapchainObjects();
    };
}