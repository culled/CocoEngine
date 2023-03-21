#pragma once

#include <Coco/Rendering/Graphics/Resources/GraphicsPresenter.h>

#include <Coco/Core/Types/List.h>
#include "../VulkanIncludes.h"
#include "../../../Resources/ImageTypes.h"

namespace Coco::Rendering
{
	class GraphicsDevice;
}

namespace Coco::Rendering::Vulkan
{
	class GraphicsDeviceVulkan;
	class GraphicsFenceVulkan;
	class GraphicsSemaphoreVulkan;
	class CommandBufferVulkan;
	class BufferVulkan;
	class ImageVulkan;
	class RenderContextVulkan;

	/// <summary>
	/// Support details for a swapchain
	/// </summary>
	struct SwapchainSupportDetails
	{
		VkSurfaceCapabilitiesKHR SurfaceCapabilities;
		List<VkSurfaceFormatKHR> SurfaceFormats;
		List<VkPresentModeKHR> PresentModes;
	};

    /// <summary>
    /// Vulkan implentation of a GraphicsPresenter
    /// </summary>
    class GraphicsPresenterVulkan final : public GraphicsPresenter
    {
	private:
		GraphicsDeviceVulkan* _device;

		VerticalSyncMode _vsyncMode = VerticalSyncMode::Enabled;
		SizeInt _backbufferSize;
		ImageDescription _backbufferDescription;

		VkSurfaceKHR _surface = nullptr;
		VkSwapchainKHR _swapchain = nullptr;

		bool _isSwapchainDirty = true;

		List<WeakManagedRef<ImageVulkan>> _backbuffers;
		List<WeakManagedRef<RenderContextVulkan>> _renderContexts;
		
		uint _currentFrame = 0;

	public:
		GraphicsPresenterVulkan(GraphicsDevice* device);
		~GraphicsPresenterVulkan() final;

		void InitializeSurface(PresenterSurfaceInitializationInfo* surfaceInitInfo) final;
		bool IsSurfaceInitialized() const noexcept final { return _surface != nullptr; }

		void SetBackbufferSize(const SizeInt& backbufferSize) noexcept final;
		SizeInt GetBackbufferSize() const noexcept final { return _backbufferSize; }

		void SetVSyncMode(VerticalSyncMode mode) noexcept final;
		VerticalSyncMode GetVSyncMode() const noexcept final { return _vsyncMode; }

		bool GetRenderContext(RenderContext*& renderContext) final;
		bool Present(RenderContext* renderContext) final;

	private:
		/// <summary>
		/// Picks a present mode to use from the supported modes
		/// </summary>
		/// <param name="preferredVSyncMode">The desired vsync mode</param>
		/// <param name="supportDetails">Swapchain support details</param>
		/// <returns>The present mode to use</returns>
		static VkPresentModeKHR PickPresentMode(VerticalSyncMode preferredVSyncMode, const SwapchainSupportDetails& supportDetails) noexcept;

		/// <summary>
		/// Picks a surface format to use from the supported formats
		/// </summary>
		/// <param name="supportDetails">Swapchain support details</param>
		/// <returns>The surface format to use</returns>
		static VkSurfaceFormatKHR PickSurfaceFormat(const SwapchainSupportDetails& supportDetails) noexcept;

		/// <summary>
		/// Picks an extent for the backbuffer to use from the supported capabilities
		/// </summary>
		/// <param name="preferredSize">The desired size of the backbuffer</param>
		/// <param name="supportDetails">Swapchain support details</param>
		/// <returns>The extent to use</returns>
		static VkExtent2D PickBackbufferExtent(const SizeInt& preferredSize, const SwapchainSupportDetails& supportDetails) noexcept;

		/// <summary>
		/// Picks the number of backbuffers to use from the supported capabilities
		/// </summary>
		/// <param name="preferredCount">The desired number of images</param>
		/// <param name="supportDetails">Swapchain support details</param>
		/// <returns>The number of images to use</returns>
		static uint32_t PickBackbufferCount(int preferredCount, const SwapchainSupportDetails& supportDetails) noexcept;

		/// <summary>
		/// Gets details about what the swapchain supports
		/// </summary>
		/// <param name="device">The physical device</param>
		/// <param name="surface">The surface</param>
		/// <returns>The swapchain support details</returns>
		static SwapchainSupportDetails GetSwapchainSupportDetails(VkPhysicalDevice device, VkSurfaceKHR surface);

		/// <summary>
		/// Creates or recreates the swapchain using the currently set parameters
		/// </summary>
		bool EnsureSwapchainIsUpdated();

		/// <summary>
		/// Creates/recreates the swapchain
		/// </summary>
		/// <param name="vsyncMode">The vsync mode to use</param>
		/// <param name="backbufferSize">The size of the backbuffers to use</param>
		/// <param name="backbufferCount">The number of backbuffers to create</param>
		/// <returns>True if the swapchain was created</returns>
		bool CreateSwapchain(VerticalSyncMode vsyncMode, const SizeInt& backbufferSize, int backbufferCount);

		/// <summary>
		/// Destroys all objects created from a swapchain
		/// </summary>
		void DestroySwapchainObjects() noexcept;

		/// <summary>
		/// Creates/recreates the render contexts used for rendering frames
		/// </summary>
		void RecreateRenderContexts();

		/// <summary>
		/// Destroys the current render contexts
		/// </summary>
		void DestroyRenderContexts() noexcept;
    };
}
