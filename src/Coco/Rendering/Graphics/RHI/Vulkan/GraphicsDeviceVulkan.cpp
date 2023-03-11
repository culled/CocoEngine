#include "GraphicsDeviceVulkan.h"

#include <Coco/Core/Types/Set.h>
#include "GraphicsPlatformVulkan.h"
#include "VulkanUtilities.h"

#include <limits>

namespace Coco::Rendering
{
	PhysicalDeviceRanking::PhysicalDeviceRanking(VkPhysicalDevice device, int score) :
		Device(device), Score(score)
	{}

	GraphicsDeviceVulkan::GraphicsDeviceVulkan(GraphicsPlatformVulkan* platform, VkPhysicalDevice physicalDevice, const GraphicsDeviceCreationParameters& createParams) :
		VulkanPlatform(platform), _physicalDevice(physicalDevice)
	{
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

		_name = string(deviceProperties.deviceName);
		_deviceType = ToGraphicsDeviceType(deviceProperties.deviceType);
		_driverVersion = ToVersion(deviceProperties.driverVersion);
		_apiVersion = ToVersion(deviceProperties.apiVersion);

		// Create unique queues
		PhysicalDeviceQueueFamilyInfo queueFamilies = GetQueueFamilyInfo(physicalDevice);
		Set<int> uniqueQueueFamilies;

		if (queueFamilies.GraphicsQueueFamily.has_value())
			uniqueQueueFamilies.insert(queueFamilies.GraphicsQueueFamily.value());

		if (queueFamilies.TransferQueueFamily.has_value())
			uniqueQueueFamilies.insert(queueFamilies.TransferQueueFamily.value());

		if (queueFamilies.ComputeQueueFamily.has_value())
			uniqueQueueFamilies.insert(queueFamilies.ComputeQueueFamily.value());

		float queuePriorities = 1.0f;

		List<VkDeviceQueueCreateInfo> queueCreateInfos;

		for (const int& queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueCount = 1; // TODO: multiple queues?
			queueCreateInfo.queueFamilyIndex = static_cast<uint32_t>(queueFamily);
			queueCreateInfo.pQueuePriorities = &queuePriorities;

			queueCreateInfos.Add(queueCreateInfo);
		}

		List<const char*> enabledExtensions;

		if (createParams.SupportsPresentation)
			enabledExtensions.Add(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

		// TODO: enable features
		VkPhysicalDeviceFeatures deviceFeatures = {};

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = queueCreateInfos.Data();
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.Count());
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.ppEnabledExtensionNames = enabledExtensions.Data();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.Count());
		createInfo.enabledLayerCount = 0;
		
		AssertVkResult(vkCreateDevice(physicalDevice, &createInfo, nullptr, &_device));

		for (const int& queueFamily : uniqueQueueFamilies)
		{
			VkQueue queue;
			vkGetDeviceQueue(_device, static_cast<uint32_t>(queueFamily), 0, &queue);
			Ref<VulkanQueue> queueRef = CreateRef<VulkanQueue>(queue, queueFamily);

			if (!_graphicsQueue.has_value() && queueFamilies.GraphicsQueueFamily.has_value() && queueFamilies.GraphicsQueueFamily.value() == queueFamily)
			{
				_graphicsQueue = queueRef;
				_graphicsCommandPool = CreateManaged<CommandBufferPoolVulkan>(this, _graphicsQueue.value());
			}

			if (!_transferQueue.has_value() && queueFamilies.TransferQueueFamily.has_value() && queueFamilies.TransferQueueFamily.value() == queueFamily)
			{
				_transferQueue = queueRef;
				_transferCommandPool = CreateManaged<CommandBufferPoolVulkan>(this, _transferQueue.value());
			}

			if (!_computeQueue.has_value() && queueFamilies.ComputeQueueFamily.has_value() && queueFamilies.ComputeQueueFamily.value() == queueFamily)
			{
				_computeQueue = queueRef;
				_computeCommandPool = CreateManaged<CommandBufferPoolVulkan>(this, _computeQueue.value());
			}
		}

		_renderCache = CreateManaged<VulkanRenderCache>(this);

		LogInfo(platform->GetLogger(), FormattedString("Using Vulkan on {} - Driver version {}, API version {}", _name, _driverVersion.ToString(), _apiVersion.ToString()));
	}

	GraphicsDeviceVulkan::~GraphicsDeviceVulkan()
	{
		// Wait for any async work to finish
		WaitForIdle();

		_graphicsQueue.reset();
		_transferQueue.reset();
		_computeQueue.reset();
		_presentQueue.reset();

		if (_graphicsCommandPool.has_value())
			_graphicsCommandPool.value().reset();

		if (_transferCommandPool.has_value())
			_transferCommandPool.value().reset();

		if (_computeCommandPool.has_value())
			_computeCommandPool.value().reset();

		_renderCache.reset();

		LogTrace(VulkanPlatform->GetLogger(), FormattedString("Releasing {} resources", Resources.Count()));
		Resources.Clear();

		if (_device != nullptr)
		{
			vkDestroyDevice(_device, nullptr);
			_device = nullptr;
		}

		_physicalDevice = nullptr;

		LogTrace(VulkanPlatform->GetLogger(), "Destroyed Vulkan device");
	}

	void GraphicsDeviceVulkan::WaitForIdle()
	{
		vkDeviceWaitIdle(_device);
	}

	Managed<GraphicsDeviceVulkan> GraphicsDeviceVulkan::Create(GraphicsPlatformVulkan* platform, const GraphicsDeviceCreationParameters& createParams)
	{
		VkPhysicalDevice physicalDevice = PickPhysicalDevice(platform->GetInstance(), createParams);
		return CreateManaged<GraphicsDeviceVulkan>(platform, physicalDevice, createParams);
	}

	bool GraphicsDeviceVulkan::InitializePresentQueue(VkSurfaceKHR surface)
	{
		if (_presentQueue.has_value())
			return true;

		if (_graphicsQueue.has_value() && CheckQueuePresentSupport(surface, _graphicsQueue.value()))
		{
			_presentQueue = _graphicsQueue.value();
		}
		else if (_transferQueue.has_value() && CheckQueuePresentSupport(surface, _transferQueue.value()))
		{
			_presentQueue = _transferQueue.value();
		}
		else if (_computeQueue.has_value() && CheckQueuePresentSupport(surface, _computeQueue.value()))
		{
			_presentQueue = _computeQueue.value();
		}

		return _presentQueue.has_value();
	}

	bool GraphicsDeviceVulkan::GetGraphicsQueue(Ref<VulkanQueue>& graphicsQueue) const
	{
		if (_graphicsQueue.has_value())
		{
			graphicsQueue = _graphicsQueue.value();
			return true;
		}

		return false;
	}

	bool GraphicsDeviceVulkan::GetTransferQueue(Ref<VulkanQueue>& transferQueue) const
	{
		if (_transferQueue.has_value())
		{
			transferQueue = _transferQueue.value();
			return true;
		}

		return false;
	}

	bool GraphicsDeviceVulkan::GetComputeQueue(Ref<VulkanQueue>& computeQueue) const
	{
		if (_computeQueue.has_value())
		{
			computeQueue = _computeQueue.value();
			return true;
		}

		return false;
	}

	bool GraphicsDeviceVulkan::GetPresentQueue(Ref<VulkanQueue>& presentQueue) const
	{
		if (_presentQueue.has_value())
		{
			presentQueue = _presentQueue.value();
			return true;
		}

		return false;
	}

	bool GraphicsDeviceVulkan::GetGraphicsCommandPool(CommandBufferPoolVulkan** poolPtr) const
	{
		if (_graphicsCommandPool.has_value())
		{
			*poolPtr = _graphicsCommandPool.value().get();
			return true;
		}

		poolPtr = nullptr;
		return false;
	}

	bool GraphicsDeviceVulkan::GetTransferCommandPool(CommandBufferPoolVulkan** poolPtr) const
	{
		if (_transferCommandPool.has_value())
		{
			*poolPtr = _transferCommandPool.value().get();
			return true;
		}

		poolPtr = nullptr;
		return false;
	}

	bool GraphicsDeviceVulkan::GetComputeCommandPool(CommandBufferPoolVulkan** poolPtr) const
	{
		if (_computeCommandPool.has_value())
		{
			*poolPtr = _computeCommandPool.value().get();
			return true;
		}

		poolPtr = nullptr;
		return false;
	}

	int GraphicsDeviceVulkan::FindMemoryIndex(uint32_t type, VkMemoryPropertyFlags memoryProperties) const
	{
		VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
		vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &deviceMemoryProperties);

		for (uint i = 0; i < deviceMemoryProperties.memoryTypeCount; i++)
		{
			if (type & (1 << i) && (deviceMemoryProperties.memoryTypes[i].propertyFlags & memoryProperties) == memoryProperties)
			{
				return static_cast<int>(i);
			}
		}

		LogWarning(VulkanPlatform->GetLogger(), "Unable to find suitable memory type");
		return -1;
	}

	bool CompareDeviceRankings(const PhysicalDeviceRanking& a, const PhysicalDeviceRanking& b)
	{
		return a.Score > b.Score;
	}

	VkPhysicalDevice GraphicsDeviceVulkan::PickPhysicalDevice(VkInstance instance, const GraphicsDeviceCreationParameters& createParams)
	{
		uint32_t deviceCount;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		List<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.Data());

		List<PhysicalDeviceRanking> deviceRankings;

		for (const VkPhysicalDevice& device : devices)
		{
			int score = CalculateDeviceScore(device, createParams);
			deviceRankings.Add(PhysicalDeviceRanking(device, score));
		}

		std::sort(deviceRankings.begin(), deviceRankings.end(), CompareDeviceRankings);

		const PhysicalDeviceRanking& firstRank = deviceRankings[0];

		if (firstRank.Score == -1)
			throw Exception("No GPUs found that satisfy the creation requirements");

		return firstRank.Device;
	}

	int GraphicsDeviceVulkan::CalculateDeviceScore(VkPhysicalDevice device, const GraphicsDeviceCreationParameters& createParams)
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
		PhysicalDeviceQueueFamilyInfo queueFamilies = GetQueueFamilyInfo(device);

		if (createParams.RequireGraphicsCapability)
		{
			if (queueFamilies.GraphicsQueueFamily.has_value())
				score++;
			else
				isMissingCriticalRequirement = true;
		}

		if (createParams.RequireTransferCapability)
		{
			if (queueFamilies.TransferQueueFamily.has_value())
				score++;
			else
				isMissingCriticalRequirement = true;
		}

		if (createParams.RequireComputeCapability)
		{
			if (queueFamilies.ComputeQueueFamily.has_value())
				score++;
			else
				isMissingCriticalRequirement = true;
		}

		// Check device extensions
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		List<VkExtensionProperties> deviceExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, deviceExtensions.Data());

		List<string> requiredExtensions;

		if (createParams.SupportsPresentation)
			requiredExtensions.Add(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

		for (const string& requiredExtension : requiredExtensions)
		{
			bool found = false;

			for (const VkExtensionProperties& properties : deviceExtensions)
			{
				if (strcmp(properties.extensionName, requiredExtension.c_str()) == 0)
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

	PhysicalDeviceQueueFamilyInfo GraphicsDeviceVulkan::GetQueueFamilyInfo(VkPhysicalDevice device)
	{
		PhysicalDeviceQueueFamilyInfo queueFamilyInfos = {};

		uint32_t queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		List<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyProperties.Data());

		int graphicsIndex = -1;
		int transferIndex = -1;
		int computeIndex = -1;

		int minimumTransferScore = std::numeric_limits<int>::max();

		for (int i = 0; i < static_cast<int>(queueFamilyCount); i++)
		{
			int transferScore = 0;

			if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				if(graphicsIndex == -1)
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

	bool GraphicsDeviceVulkan::CheckQueuePresentSupport(VkSurfaceKHR surface, const Ref<VulkanQueue>& queue) const
	{
		VkBool32 supported = VK_FALSE;

		vkGetPhysicalDeviceSurfaceSupportKHR(_physicalDevice, static_cast<uint32_t>(queue->QueueFamily), surface, &supported);

		return supported != VK_FALSE;
	}
}