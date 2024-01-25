#pragma once
#include "../../GraphicsDevice.h"

#include <Coco/Core/MainLoop/TickListener.h>
#include "VulkanRenderFrame.h"
#include "VulkanQueue.h"
#include "VulkanGraphicsDeviceCache.h"
#include "VulkanIncludes.h"
#include <vk_mem_alloc.h>

namespace Coco::Rendering::Vulkan
{
	class VulkanGraphicsPlatform;
	struct VulkanGraphicsDeviceCreateParams;

	/// @brief Queue families that can be used on a physical device
	struct VkPhysicalDeviceQueueFamilyInfo
	{
		/// @brief The index of the graphics queue family, if any
		std::optional<uint32> GraphicsQueueFamily;

		/// @brief The index of the transfer queue family, if any
		std::optional<uint32> TransferQueueFamily;

		/// @brief The index of the compute queue family, if any
		std::optional<uint32> ComputeQueueFamily;

		VkPhysicalDeviceQueueFamilyInfo();

		static VkPhysicalDeviceQueueFamilyInfo GetInfo(VkPhysicalDevice physicalDevice);
	};

	struct VulkanGraphicsDeviceFeatures :
		public GraphicsDeviceFeatures
	{
		/// @brief The maximum size of the push constants buffer
		uint32 MaxPushConstantSize;

		VulkanGraphicsDeviceFeatures();
	};

	class VulkanGraphicsDevice :
		public GraphicsDevice
	{
	public:
		static const double ResourcePurgePeriod;
		static const double StaleResourceThreshold;
		static const int ResourcePurgeTickPriority;

	public:
		VulkanGraphicsDevice(const VulkanGraphicsPlatform& platform, const VulkanGraphicsDeviceCreateParams& createParams, VkPhysicalDevice physicalDevice);
		~VulkanGraphicsDevice();

		// Inherited via GraphicsDevice
		const string& GetName() const override { return _features.Name; }
		GraphicsDeviceType GetDeviceType() const override { return _features.Type; }
		Version GetDriverVersion() const override { return _features.DriverVersion; }
		const GraphicsDeviceFeatures& GetFeatures() const { return _features; }
		uint32 GetDataTypeAlignment(BufferDataType type) const;
		Ref<Presenter> CreatePresenter() override;
		Ref<Buffer> CreateBuffer(uint64 size, BufferUsageFlags usageFlags) override;
		Ref<Image> CreateImage(const ImageDescription& description) override;
		Ref<ImageSampler> CreateImageSampler(const ImageSamplerDescription& description) override;
		bool TryReleaseResource(const GraphicsResourceID& resourceID) override;
		SharedRef<RenderFrame> GetCurrentRenderFrame() const override { return _currentRenderFrame; }
		void WaitForIdle() override;

		static bool PickPhysicalDevice(VkInstance instance, const VulkanGraphicsDeviceCreateParams& createParams, VkPhysicalDevice& outDevice);

		VkInstance GetVulkanInstance() const;
		VkDevice GetDevice() const { return _device; }
		VkPhysicalDevice GetPhysicalDevice() const { return _physicalDevice; }
		const VkAllocationCallbacks* GetAllocationCallbacks() const;
		VmaAllocator GetAllocator() const { return _allocator; }

		VulkanQueue* GetQueue(VulkanQueueType queueType);
		VulkanQueue* GetOrCreatePresentQueue(VkSurfaceKHR surface);

		VulkanGraphicsDeviceCache& GetCache() { return *_cache; }
		Ref<VulkanRenderContext> CreateRenderContext();

	protected:
		// Inherited via GraphicsDevice
		SharedRef<RenderFrame> StartNewRenderFrame() override;
		void EndRenderFrame() override;

	private:
		static std::atomic_uint64_t _sNextResourceID;

		const VulkanGraphicsPlatform& _platform;
		VkPhysicalDevice _physicalDevice;
		VkDevice _device;
		VmaAllocator _allocator;
		VulkanGraphicsDeviceFeatures _features;
		ManagedRef<TickListener> _purgeTickListener;

		UniqueRef<VulkanQueue> _graphicsQueue;
		UniqueRef<VulkanQueue> _transferQueue;
		UniqueRef<VulkanQueue> _computeQueue;
		VulkanQueue* _presentQueue;

		UniqueRef<VulkanGraphicsDeviceCache> _cache;

		SharedRef<VulkanRenderFrame> _currentRenderFrame;

		std::unordered_map<GraphicsResourceID, ManagedRef<GraphicsResource>> _resources;

	private:
		static VulkanGraphicsDeviceFeatures GetDeviceFeatures(VkPhysicalDevice physicalDevice);
		static int CalculateDeviceScore(const VkPhysicalDevice& device, const VulkanGraphicsDeviceCreateParams& createParams);
		static GraphicsResourceID GetNewResourceID();

		template<typename ResourceType, typename... Args>
		Ref<ResourceType> CreateResource(Args&&... args)
		{
			GraphicsResourceID id = GetNewResourceID();
			auto result = _resources.try_emplace(id, CreateManagedRef<ResourceType>(id, *this, std::forward<Args>(args)...));
			Ref<GraphicsResource> r = result.first->second;
			return StaticRefCast<ResourceType>(r);
		}

		void CreateDevice(const VulkanGraphicsDeviceCreateParams& createParams);
		void DestroyDevice();

		bool CheckPresentQueueSupport(VkSurfaceKHR surface, const VulkanQueue* queue);
		void HandlePurgeTickListener(const TickInfo& tickInfo);
		void PurgeStaleResources();
	};
}