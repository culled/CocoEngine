#include "Renderpch.h"
#include "VulkanGraphicsDevice.h"

#include <Coco/Core/Core.h>
#include <Coco/Core/Engine.h>
#include "VulkanUtils.h"

namespace Coco::Rendering::Vulkan
{
	VulkanGraphicsDevice::VulkanGraphicsDevice(VkPhysicalDevice physicalDevice, const GraphicsDeviceCreateParams& createParams) :
		_physicalDevice(physicalDevice)
	{
		GetPhysicalDeviceProperties();

		CocoInfo("Using Vulkan on {} - Driver version {}, API version {}",
			_name,
			_driverVersion.ToString(),
			_apiVersion.ToString()
		)

		CocoTrace("Created VulkanGraphicsDevice")
	}

	void VulkanGraphicsDevice::GetPhysicalDeviceProperties()
	{
		// Get basic device info
		VkPhysicalDeviceProperties deviceProperties{};
		vkGetPhysicalDeviceProperties(_physicalDevice, &deviceProperties);

		_name = string(&deviceProperties.deviceName[0]);
		_deviceType = ToGraphicsDeviceType(deviceProperties.deviceType);
		_driverVersion = ToVersion(deviceProperties.driverVersion);
		_apiVersion = ToVersion(deviceProperties.apiVersion);
	}

	PhysicalDeviceQueueFamilyInfo GetQueueFamilyInfo(const VkPhysicalDevice& device) noexcept
	{
		PhysicalDeviceQueueFamilyInfo queueFamilyInfos = {};

		uint32_t queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyProperties.data());

		int graphicsIndex = -1;
		int transferIndex = -1;
		int computeIndex = -1;

		int minimumTransferScore = Math::MaxValue<int>();

		for (uint32_t i = 0; i < queueFamilyCount; i++)
		{
			int transferScore = 0;

			if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				if (graphicsIndex == -1)
					graphicsIndex = i;
				transferScore++;
			}

			if (queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
			{
				// Prioritize compute queues without graphics queues
				if (computeIndex == -1 || (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != VK_QUEUE_GRAPHICS_BIT)
					computeIndex = i;

				transferScore++;
			}

			if (queueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
			{
				// Pick the transfer queue with the lowest score,
				// that way it is likely it will not be shared with any other queues
				if (transferScore < minimumTransferScore)
				{
					transferIndex = i;
					minimumTransferScore = transferScore;
				}
			}
		}

		if (graphicsIndex != -1)
			queueFamilyInfos.GraphicsQueueFamily = graphicsIndex;

		if (transferIndex != -1)
			queueFamilyInfos.TransferQueueFamily = transferIndex;

		if (computeIndex != -1)
			queueFamilyInfos.ComputeQueueFamily = computeIndex;

		return queueFamilyInfos;
	}

	int CalculateDeviceScore(const VkPhysicalDevice& device, const GraphicsDeviceCreateParams& createParams) noexcept
	{
		int score = 0;
		bool isMissingCriticalRequirement = false;

		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);

		// Check device type
		if (ToGraphicsDeviceType(deviceProperties.deviceType) == createParams.PreferredDeviceType)
		{
			score += 3;
		}
		else
		{
			switch (deviceProperties.deviceType)
			{
			case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
				score += 2;
				break;
			case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
				score += 1;
				break;
			default:
				break;
			}
		}

		// Check device queues
		const PhysicalDeviceQueueFamilyInfo queueFamilies = GetQueueFamilyInfo(device);

		// Check for a graphics queue
		if (queueFamilies.GraphicsQueueFamily.has_value())
			score++;
		else if (createParams.RequireGraphicsCapability)
			isMissingCriticalRequirement = true;

		// Check for a transfer queue
		if (queueFamilies.TransferQueueFamily.has_value())
			score++;
		else if (createParams.RequireTransferCapability)
			isMissingCriticalRequirement = true;

		// Check for a compute queue
		if (queueFamilies.ComputeQueueFamily.has_value())
			score++;
		else if (createParams.RequireComputeCapability)
			isMissingCriticalRequirement = true;

		// Check device extensions
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> deviceExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, deviceExtensions.data());

		std::vector<string> requiredExtensions;

		if (createParams.SupportsPresentation)
			requiredExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

		for (const string& requiredExtension : requiredExtensions)
		{
			bool found = false;

			for (const VkExtensionProperties& properties : deviceExtensions)
			{
				if (strcmp(&properties.extensionName[0], requiredExtension.c_str()) == 0)
				{
					score++;
					found = true;
					break;
				}
			}

			if (!found)
			{
				isMissingCriticalRequirement = true;
				break;
			}
		}

		return isMissingCriticalRequirement ? -1 : score;
	}

	VkPhysicalDevice PickPhysicalDevice(const VkInstance& instance, const GraphicsDeviceCreateParams& createParams)
	{
		uint32_t deviceCount;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		std::vector<PhysicalDeviceRanking> deviceRankings;

		for (const VkPhysicalDevice& device : devices)
		{
			deviceRankings.emplace_back(device, CalculateDeviceScore(device, createParams));
		}

		std::sort(deviceRankings.begin(), deviceRankings.end(), [](const PhysicalDeviceRanking& a, const PhysicalDeviceRanking& b) {
			return a.Score > b.Score;
			});

		const PhysicalDeviceRanking& firstRank = deviceRankings[0];

		return firstRank.Score == -1 ? nullptr : firstRank.Device;
	}

	PhysicalDeviceRanking::PhysicalDeviceRanking(VkPhysicalDevice device, int score) :
		Device(device), Score(score)
	{}

	VulkanGraphicsDevice::~VulkanGraphicsDevice()
	{
		CocoTrace("Destroyed VulkanGraphicsDevice")
	}

	UniqueRef<VulkanGraphicsDevice> VulkanGraphicsDevice::Create(const VkInstance& instance, const GraphicsDeviceCreateParams& createParams)
	{
		VkPhysicalDevice device = PickPhysicalDevice(instance, createParams);

		if (!device)
		{
			CocoCritical("Could not find a graphics device that satisfies the given creating requirements")
			return nullptr;
		}

		return CreateUniqueRef<VulkanGraphicsDevice>(device, createParams);
	}
}