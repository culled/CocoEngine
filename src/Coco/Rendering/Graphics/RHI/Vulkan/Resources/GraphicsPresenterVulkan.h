#pragma once

#include <Coco/Rendering/Graphics/Resources/GraphicsPresenter.h>
#include <Coco/Rendering/Graphics/Resources/GraphicsResource.h>

#include <Coco/Core/Types/List.h>
#include "../VulkanIncludes.h"
#include "../../../Resources/ImageTypes.h"

namespace Coco::Rendering::Vulkan
{
	class GraphicsDeviceVulkan;
	class GraphicsFenceVulkan;
	class GraphicsSemaphoreVulkan;
	class CommandBufferVulkan;
	class BufferVulkan;
	class ImageVulkan;
	class RenderContextVulkan;

	/// @brief Support details for a swapchain
	struct SwapchainSupportDetails
	{
		/// @brief The surface capabilities
		VkSurfaceCapabilitiesKHR SurfaceCapabilities;

		/// @brief A list of supported surface formats
		List<VkSurfaceFormatKHR> SurfaceFormats;

		/// @brief A list of supported present modes
		List<VkPresentModeKHR> PresentModes;
	};

    /// @brief Vulkan implentation of a GraphicsPresenter
    class GraphicsPresenterVulkan final : public GraphicsResource<GraphicsDeviceVulkan, GraphicsPresenter>
    {
		friend class ManagedRef<GraphicsPresenterVulkan>;

	private:
		VerticalSyncMode _vsyncMode = VerticalSyncMode::Enabled;
		SizeInt _backbufferSize;
		ImageDescription _backbufferDescription;

		VkSurfaceKHR _surface = nullptr;
		VkSwapchainKHR _swapchain = nullptr;

		bool _isSwapchainDirty = true;

		List<Ref<ImageVulkan>> _backbuffers;
		List<Ref<RenderContextVulkan>> _renderContexts;
		List<int> _acquiredBackBufferIndices;
		
		uint _currentFrame = 0;

	protected:
		GraphicsPresenterVulkan(const ResourceID& id, const string& name);

	public:
		~GraphicsPresenterVulkan() final;

		DefineResourceType(GraphicsPresenterVulkan)

		void InitializeSurface(const PresenterSurfaceInitializationInfo& surfaceInitInfo) final;
		bool IsSurfaceInitialized() const noexcept final { return _surface != nullptr; }

		void SetBackbufferSize(const SizeInt& backbufferSize) noexcept final;
		SizeInt GetBackbufferSize() const noexcept final { return _backbufferSize; }

		void SetVSyncMode(VerticalSyncMode mode) noexcept final;
		VerticalSyncMode GetVSyncMode() const noexcept final { return _vsyncMode; }

		bool PrepareForRender(Ref<RenderContext>& renderContext, Ref<Image>& backbuffer) final;
		bool Present(Ref<RenderContext> renderContext) final;

	private:
		/// @brief Picks a present mode to use from the supported modes
		/// @param preferredVSyncMode The desired vsync mode
		/// @param supportDetails Swapchain support details
		/// @return The present mode to use
		static VkPresentModeKHR PickPresentMode(VerticalSyncMode preferredVSyncMode, const SwapchainSupportDetails& supportDetails) noexcept;

		/// @brief Picks a surface format to use from the supported formats
		/// @param supportDetails Swapchain support details
		/// @return The surface format to use
		static VkSurfaceFormatKHR PickSurfaceFormat(const SwapchainSupportDetails& supportDetails) noexcept;

		/// @brief Picks an extent for the backbuffer to use from the supported capabilities
		/// @param preferredSize The desired size of the backbuffer
		/// @param supportDetails Swapchain support details
		/// @return The extent to use
		static VkExtent2D PickBackbufferExtent(const SizeInt& preferredSize, const SwapchainSupportDetails& supportDetails) noexcept;

		/// @brief Picks the number of backbuffers to use from the supported capabilities
		/// @param preferredCount The desired number of images
		/// @param supportDetails Swapchain support details
		/// @return The number of images to use
		static uint32_t PickBackbufferCount(int preferredCount, const SwapchainSupportDetails& supportDetails) noexcept;

		/// @brief Gets details about what the swapchain supports
		/// @param device The physical device
		/// @param surface The surface
		/// @return The swapchain support details
		static SwapchainSupportDetails GetSwapchainSupportDetails(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);

		/// @brief Creates or recreates the swapchain using the currently set parameters
		/// @return True if the swapchain is up to date
		bool EnsureSwapchainIsUpdated();

		/// @brief Creates/recreates the swapchain
		/// @param vsyncMode The vsync mode to use
		/// @param backbufferSize The size of the backbuffers to use
		/// @param backbufferCount The number of backbuffers to create
		/// @return True if the swapchain was created
		bool CreateSwapchain(VerticalSyncMode vsyncMode, const SizeInt& backbufferSize, int backbufferCount);

		/// @brief Destroys all objects created from a swapchain
		void DestroySwapchainObjects() noexcept;

		/// @brief Creates/recreates the render contexts used for rendering frames
		void RecreateRenderContexts();

		/// @brief Destroys the current render contexts
		void DestroyRenderContexts() noexcept;
    };
}
