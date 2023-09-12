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
	};

	/// @brief Vulkan implementation of a GraphicsDevice
	class VulkanGraphicsDevice : public GraphicsDevice
	{
	private:
		VkPhysicalDevice _physicalDevice;
		VkDevice _device;
		string _name;
		GraphicsDeviceType _deviceType;
		Version _driverVersion;
		Version _apiVersion;

	public:
		VulkanGraphicsDevice(VkPhysicalDevice physicalDevice, const GraphicsDeviceCreateParams& createParams);
		~VulkanGraphicsDevice();

		/// @brief Creates a VulkanGraphicsDevice
		/// @param instance The vulkan instance
		/// @param createParams Parameters for creating the device
		/// @return The device
		static UniqueRef<VulkanGraphicsDevice> Create(const VkInstance& instance, const GraphicsDeviceCreateParams& createParams);

		const char* GetName() const final { return _name.c_str(); }
		GraphicsDeviceType GetDeviceType() const final { return _deviceType; }
		Version GetDriverVersion() const final { return _driverVersion; }
		Version GetAPIVersion() const final { return _apiVersion; }

	private:
		/// @brief Gets properties for this device's physical device
		void GetPhysicalDeviceProperties();
	};
}