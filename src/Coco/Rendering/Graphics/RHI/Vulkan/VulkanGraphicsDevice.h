#pragma once

#include <Coco/Rendering/Graphics/GraphicsDevice.h>
#include <Coco/Core/Core.h>
#include "VulkanIncludes.h"
#include <Coco/Core/Types/Version.h>

namespace Coco::Rendering::Vulkan
{
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

		DeviceQueue(Type type, uint8 familyIndex, VkQueue queue);
	};

	/// @brief Vulkan implementation of a GraphicsDevice
	class VulkanGraphicsDevice : public GraphicsDevice
	{
	private:
		VkInstance _instance;
		VkPhysicalDevice _physicalDevice;
		VkDevice _device;
		string _name;
		GraphicsDeviceType _deviceType;
		Version _driverVersion;
		Version _apiVersion;
		GraphicsDeviceMemoryFeatures _memoryFeatures;
		UniqueRef<DeviceQueue> _graphicsQueue;
		UniqueRef<DeviceQueue> _transferQueue;
		UniqueRef<DeviceQueue> _computeQueue;
		DeviceQueue* _presentQueue;

	public:
		VulkanGraphicsDevice(VkInstance instance, VkPhysicalDevice physicalDevice, const GraphicsDeviceCreateParams& createParams);
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
		const GraphicsDeviceMemoryFeatures& GetMemoryFeatures() const final { return _memoryFeatures; }
		void WaitForIdle() const final;

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

		/// @brief Gets or creates a queue that can be used for presentation operations
		/// @param surface The surface to check presentation support for
		/// @return The present queue, or nullptr if presentation isn't supported
		DeviceQueue* GetOrCreatePresentQueue(VkSurfaceKHR surface);

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