#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Rendering/Graphics/GraphicsPresenter.h>
#include "ImageVulkan.h"

#include "VulkanIncludes.h"

namespace Coco::Rendering
{
	class GraphicsFenceVulkan;
	class GraphicsSemaphoreVulkan;
	class CommandBufferVulkan;

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
	/// Container for backbuffer objects
	/// </summary>
	struct Backbuffer
	{
		ImageVulkan* Image = nullptr;
		GraphicsSemaphoreVulkan* ImageAvailableSemaphore = nullptr;
		GraphicsSemaphoreVulkan* RenderingCompletedSemaphore = nullptr;
		GraphicsFenceVulkan* RenderingCompletedFence = nullptr;
	};

	class GraphicsDeviceVulkan;

    /// <summary>
    /// Vulkan implentation of a GraphicsPresenter
    /// </summary>
    class GraphicsPresenterVulkan : public GraphicsPresenter
    {
	private:
		GraphicsDeviceVulkan* _device;

		VerticalSyncMode _vsyncMode = VerticalSyncMode::Enabled;
		SizeInt _backbufferSize;
		ImageDescription _backbufferDescription;

		VkSurfaceKHR _surface = nullptr;
		VkSwapchainKHR _swapchain = nullptr;

		bool _isSwapchainDirty = true;

		List<Backbuffer> _backbuffers;

		List<CommandBufferVulkan*> _commandBuffers;
		List<VkFramebuffer> _framebuffers;
		WeakRef<RenderPipeline> _framebufferPipeline;
		
		int _currentFrame = 0;

	public:
		GraphicsPresenterVulkan(GraphicsDeviceVulkan* device);
		virtual ~GraphicsPresenterVulkan() override;

		virtual void InitializeSurface(PresenterSurfaceInitializationInfo* surfaceInitInfo) override;
		virtual bool IsSurfaceInitialized() const override { return _surface != nullptr; }

		virtual void SetBackbufferSize(const SizeInt& backbufferSize) override;
		virtual SizeInt GetBackbufferSize() const override { return _backbufferSize; }

		virtual void SetVSyncMode(VerticalSyncMode mode) override;
		virtual VerticalSyncMode GetVSyncMode() const override { return _vsyncMode; }

		virtual Managed<RenderContext> CreateRenderContext(const Ref<RenderView>& view, const Ref<RenderPipeline>& pipeline, int backbufferImageIndex) override;

		virtual GraphicsPresenterResult AcquireNextBackbuffer(
			unsigned long long timeoutNs,
			int& backbufferImageIndex) override;

		virtual GraphicsPresenterResult Present(int backbufferImageIndex) override;

	private:
		/// <summary>
		/// Picks a present mode to use from the supported modes
		/// </summary>
		/// <param name="preferredVSyncMode">The desired vsync mode</param>
		/// <param name="supportDetails">Swapchain support details</param>
		/// <returns>The present mode to use</returns>
		static VkPresentModeKHR PickPresentMode(VerticalSyncMode preferredVSyncMode, const SwapchainSupportDetails& supportDetails);

		/// <summary>
		/// Picks a surface format to use from the supported formats
		/// </summary>
		/// <param name="supportDetails">Swapchain support details</param>
		/// <returns>The surface format to use</returns>
		static VkSurfaceFormatKHR PickSurfaceFormat(const SwapchainSupportDetails& supportDetails);

		/// <summary>
		/// Picks an extent for the backbuffer to use from the supported capabilities
		/// </summary>
		/// <param name="preferredSize">The desired size of the backbuffer</param>
		/// <param name="supportDetails">Swapchain support details</param>
		/// <returns>The extent to use</returns>
		static VkExtent2D PickBackbufferExtent(const SizeInt& preferredSize, const SwapchainSupportDetails& supportDetails);

		/// <summary>
		/// Picks the number of backbuffers to use from the supported capabilities
		/// </summary>
		/// <param name="preferredCount">The desired number of images</param>
		/// <param name="supportDetails">Swapchain support details</param>
		/// <returns>The number of images to use</returns>
		static uint32_t PickBackbufferCount(int preferredCount, const SwapchainSupportDetails& supportDetails);

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
		void EnsureSwapchainIsUpdated();

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
		void DestroySwapchainObjects();

		/// <summary>
		/// Gets the backbuffer images from the swapchain
		/// </summary>
		/// <returns>True if the images were obtained</returns>
		bool GetBackbufferImages();

		/// <summary>
		/// Creates/recreates the framebuffers for a given render pipeline and renderpass combo
		/// </summary>
		/// <param name="pipeline">The render pipeline</param>
		/// <param name="renderPass">The renderpass</param>
		void RecreateFramebuffers(const Ref<RenderPipeline>& pipeline, VkRenderPass renderPass);

		/// <summary>
		/// Destroys the current framebuffers
		/// </summary>
		void DestroyFramebuffers();

		/// <summary>
		/// Allocates/reallocates the command buffers used for rendering
		/// </summary>
		void ReallocateCommandBuffers();

		/// <summary>
		/// Frees the current command buffers
		/// </summary>
		void FreeCommandBuffers();
    };
}