#pragma once

#include "../../GraphicsDevice.h"
#include <Coco/Core/Resources/ResourceLibrary.h>
#include <Coco/Core/Types/String.h>
#include <Coco/Core/Types/Version.h>
#include "VulkanCommandBufferPool.h"
#include "VulkanGraphicsDeviceCache.h"
#include "VulkanShaderCache.h"
#include "VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
	class VulkanGraphicsDevice;

	/// @brief Vulkan-specific device features
	struct VulkanGraphicsDeviceFeatures
	{
		/// @brief The maximum size of the push constants buffer
		uint32 MaxPushConstantSize;

		VulkanGraphicsDeviceFeatures();
	};

	/// @brief A ranking for a VkPhysicalDevice
	struct PhysicalDeviceRanking
	{
		/// @brief The physical device
		VkPhysicalDevice Device;

		/// @brief The device's score
		int Score;

		PhysicalDeviceRanking(VkPhysicalDevice device, int score);
	};

	/// @brief Queue families that can be used on a physical device
	struct PhysicalDeviceQueueFamilyInfo
	{
		/// @brief The index of the graphics queue family, if any
		std::optional<int> GraphicsQueueFamily;

		/// @brief The index of the transfer queue family, if any
		std::optional<int> TransferQueueFamily;

		/// @brief The index of the compute queue family, if any
		std::optional<int> ComputeQueueFamily;

		PhysicalDeviceQueueFamilyInfo();
	};

	/// @brief Describes a queue on the VulkanGraphicsDevice
	struct DeviceQueue
	{
		/// @brief Types of queues
		enum class Type
		{
			Graphics,
			Transfer,
			Compute
		};

		/// @brief The type of queue this is
		Type QueueType;

		/// @brief The family index of the queue
		uint8 FamilyIndex;

		/// @brief The actual queue
		VkQueue Queue;

		/// @brief The command buffer pool
		VulkanCommandBufferPool Pool;

		DeviceQueue(VulkanGraphicsDevice& device, Type type, uint8 familyIndex, VkQueue queue);
	};

	/// @brief Vulkan implementation of a GraphicsDevice
	class VulkanGraphicsDevice :
		public GraphicsDevice
	{
	public:
		/// @brief The period between purges in seconds
		static const double sPurgePeriod;

	private:
		VkInstance _instance;
		VkPhysicalDevice _physicalDevice;
		VkDevice _device;
		string _name;
		GraphicsDeviceType _deviceType;
		Version _driverVersion;
		Version _apiVersion;
		GraphicsDeviceFeatures _features;
		VulkanGraphicsDeviceFeatures _vulkanFeatures;
		UniqueRef<DeviceQueue> _graphicsQueue;
		UniqueRef<DeviceQueue> _transferQueue;
		UniqueRef<DeviceQueue> _computeQueue;
		DeviceQueue* _presentQueue;
		UniqueRef<VulkanGraphicsDeviceCache> _cache;
		UniqueRef<VulkanShaderCache> _shaderCache;
		OwnedResourceLibrary<GraphicsDeviceResourceID, GraphicsDeviceResourceBase, GraphicsDeviceResourceIDGenerator> _resources;
		double _lastPurgeTime;

	public:
		VulkanGraphicsDevice(VkInstance instance, const GraphicsDeviceCreateParams& createParams, VkPhysicalDevice physicalDevice);
		~VulkanGraphicsDevice();

		/// @brief Creates a VulkanGraphicsDevice
		/// @param instance The vulkan instance
		/// @param createParams Parameters for creating the device
		/// @return The device
		static UniqueRef<VulkanGraphicsDevice> Create(VkInstance instance, const GraphicsDeviceCreateParams& createParams);

		const char* GetName() const final { return _name.c_str(); }
		GraphicsDeviceType GetDeviceType() const final { return _deviceType; }
		Version GetDriverVersion() const final { return _driverVersion; }
		Version GetAPIVersion() const final { return _apiVersion; }
		const GraphicsDeviceFeatures& GetFeatures() const final { return _features; }
		void WaitForIdle() const final;
		uint8 GetDataTypeAlignment(BufferDataType type) const final;
		void AlignOffset(BufferDataType type, uint64& offset) const final;
		ShaderCache& GetShaderCache() final { return *_shaderCache; }
		Ref<GraphicsPresenter> CreatePresenter() final;
		void TryReleasePresenter(Ref<GraphicsPresenter>& presenter) final;
		Ref<Buffer> CreateBuffer(uint64 size, BufferUsageFlags usageFlags, bool bind) final;
		void TryReleaseBuffer(Ref<Buffer>& buffer) final;
		Ref<Image> CreateImage(const ImageDescription& description) final;
		void TryReleaseImage(Ref<Image>& image) final;
		Ref<ImageSampler> CreateImageSampler(const ImageSamplerDescription& description) final;
		void TryReleaseImageSampler(Ref<ImageSampler>& imageSampler) final;
		Ref<RenderContext> CreateRenderContext() final;
		void TryReleaseRenderContext(Ref<RenderContext>& context) final;
		void PurgeUnusedResources() final;
		void ResetForNewFrame() final;

		/// @brief Gets the Vulkan instance that this device was created with
		/// @return The Vulkan instance
		VkInstance GetInstance() const { return _instance; }

		/// @brief Gets this device's allocation callbacks
		/// @return This device's allocation callbacks
		VkAllocationCallbacks* GetAllocationCallbacks() { return nullptr; } // TODO: allocation callbacks

		/// @brief Gets the logical device
		/// @return The logical device
		VkDevice GetDevice() const { return _device; }

		/// @brief Gets the physical device
		/// @return The physical device
		VkPhysicalDevice GetPhysicalDevice() const { return _physicalDevice; }

		/// @brief Gets a queue on this device
		/// @param queueType The type of queue
		/// @return The queue, or nullptr if it doesn't exist
		DeviceQueue* GetQueue(DeviceQueue::Type queueType);

		/// @brief Gets a queue on this device
		/// @param queueType The type of queue
		/// @return The queue, or nullptr if it doesn't exist
		const DeviceQueue* GetQueue(DeviceQueue::Type queueType) const;

		/// @brief Gets or creates a queue that can be used for presentation operations
		/// @param surface The surface to check presentation support for
		/// @return The present queue, or nullptr if presentation isn't supported
		DeviceQueue* GetOrCreatePresentQueue(VkSurfaceKHR surface);

		/// @brief Gets this device's cache
		/// @return The device cache
		VulkanGraphicsDeviceCache& GetCache() { return *_cache; }

		/// @brief Gets the Vulkan-specific features of this device
		/// @return The Vulkan features
		const VulkanGraphicsDeviceFeatures& GetVulkanFeatures() const { return _vulkanFeatures; }

		/// @brief Finds the heap index for a type of memory
		/// @param type The memory types
		/// @param memoryProperties The memory properties
		/// @param outIndex Will be set to the heap index
		/// @return True if memory was found that supports the given properties
		bool FindMemoryIndex(uint32 type, VkMemoryPropertyFlags memoryProperties, uint32& outIndex) const;

		/// @brief Waits until a queue has finished all operations
		/// @param queueType The queue to wait for
		void WaitForQueueIdle(DeviceQueue::Type queueType) const;

		void TryReleaseResource(const uint64& id);

	private:
		/// @brief Gets queue family information for a device
		/// @param device The device
		/// @return The queue family info
		static PhysicalDeviceQueueFamilyInfo GetQueueFamilyInfo(VkPhysicalDevice device);

		/// @brief Calculates a score for a given device based on how much it satisfies the given requirements
		/// @param device The device
		/// @param createParams The requirements for the device
		/// @return The device score
		static int CalculateDeviceScore(const VkPhysicalDevice& device, const GraphicsDeviceCreateParams& createParams);

		/// @brief Picks the most suitable physical device to use for rendering
		/// @param instance The Vulkan instance
		/// @param createParams The requirements for the device
		/// @return The most suitable physical device
		static VkPhysicalDevice PickPhysicalDevice(const VkInstance& instance, const GraphicsDeviceCreateParams& createParams);

		/// @brief Creates the Vulkan logical device
		/// @param createParams The parameters to create the device with
		void CreateLogicalDevice(const GraphicsDeviceCreateParams& createParams);

		/// @brief Gets properties for this device's physical device
		void GetPhysicalDeviceProperties();

		/// @brief Checks if a given queue supports presentation
		/// @param surface The surface to check support with
		/// @param queue The queue
		/// @return True if the queue supports presenting
		bool CheckQueuePresentSupport(VkSurfaceKHR surface, const DeviceQueue* queue);
	};
}