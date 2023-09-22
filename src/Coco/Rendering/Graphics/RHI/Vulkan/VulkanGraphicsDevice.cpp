#include "Renderpch.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanGraphicsPresenter.h"
#include "VulkanBuffer.h"
#include "VulkanImage.h"
#include "VulkanImageSampler.h"
#include "../../GraphicsDeviceResource.h"

#include <Coco/Core/Core.h>
#include <Coco/Core/Engine.h>
#include "VulkanUtils.h"

namespace Coco::Rendering::Vulkan
{
	PhysicalDeviceRanking::PhysicalDeviceRanking(VkPhysicalDevice device, int score) :
		Device(device), Score(score)
	{}

	PhysicalDeviceQueueFamilyInfo::PhysicalDeviceQueueFamilyInfo() :
		GraphicsQueueFamily{},
		TransferQueueFamily{},
		ComputeQueueFamily{}
	{}

	DeviceQueue::DeviceQueue(VulkanGraphicsDevice* device, Type type, uint8 familyIndex, VkQueue queue) :
		QueueType(type),
		FamilyIndex(familyIndex),
		Queue(queue),
		Pool(device, queue, familyIndex)
	{}

	VulkanGraphicsDevice::VulkanGraphicsDevice(VkInstance instance, const GraphicsDeviceCreateParams& createParams, VkPhysicalDevice physicalDevice) :
		_instance(instance),
		_physicalDevice(physicalDevice),
		_device(nullptr),
		_memoryFeatures{},
		_graphicsQueue(nullptr),
		_transferQueue(nullptr),
		_computeQueue(nullptr),
		_presentQueue(nullptr),
		_cache(CreateUniqueRef<VulkanGraphicsDeviceCache>()),
		_resources{}
	{
		GetPhysicalDeviceProperties();
		CreateLogicalDevice(createParams);

		CocoInfo("Using Vulkan on {} - Driver version {}, API version {}",
			_name,
			_driverVersion.ToString(),
			_apiVersion.ToString()
		)

		CocoTrace("Created VulkanGraphicsDevice")
	}

	VulkanGraphicsDevice::~VulkanGraphicsDevice()
	{
		WaitForIdle();

		_cache.reset();

		_graphicsQueue.reset();
		_transferQueue.reset();
		_computeQueue.reset();
		_presentQueue = nullptr;

		CocoTrace("Destroying {} resources", _resources.GetCount())
		_resources.Clear();

		if (_device)
		{
			vkDestroyDevice(_device, GetAllocationCallbacks());
			_device = nullptr;
		}

		_physicalDevice = nullptr;

		CocoTrace("Destroyed VulkanGraphicsDevice")
	}

	UniqueRef<VulkanGraphicsDevice> VulkanGraphicsDevice::Create(VkInstance instance, const GraphicsDeviceCreateParams& createParams)
	{
		VkPhysicalDevice device = PickPhysicalDevice(instance, createParams);

		if (!device)
		{
			CocoCritical("Could not find a graphics device that satisfies the given creating requirements")
				return nullptr;
		}

		return CreateUniqueRef<VulkanGraphicsDevice>(instance, createParams, device);
	}

	void VulkanGraphicsDevice::WaitForIdle() const
	{
		vkDeviceWaitIdle(_device);
	}

	uint8 VulkanGraphicsDevice::GetDataTypeAlignment(BufferDataType type) const
	{
		switch (type)
		{
		case BufferDataType::Float:
			return BufferFloatSize;
		case BufferDataType::Float2:
			return BufferFloatSize * 2;
		case BufferDataType::Float3:
		case BufferDataType::Float4:
			return BufferFloatSize * 4;
		case BufferDataType::Int:
			return BufferIntSize;
		case BufferDataType::Int2:
			return BufferIntSize * 2;
		case BufferDataType::Int3:
		case BufferDataType::Int4:
			return BufferIntSize * 4;
		case BufferDataType::Mat4x4:
			return BufferFloatSize * 16;
		default:
			return 0;
		}
	}

	void VulkanGraphicsDevice::AlignOffset(BufferDataType type, uint64& offset) const
	{
		offset = GetOffsetForAlignment(offset, GetDataTypeAlignment(type));
	}

	Ref<GraphicsPresenter> VulkanGraphicsDevice::CreatePresenter()
	{
		return _resources.Create<VulkanGraphicsPresenter>();
	}

	Ref<Buffer> VulkanGraphicsDevice::CreateBuffer(uint64 size, BufferUsageFlags usageFlags, bool bind)
	{
		return _resources.Create<VulkanBuffer>(size, usageFlags, bind);
	}

	Ref<Image> VulkanGraphicsDevice::CreateImage(const ImageDescription& description)
	{
		return _resources.Create<VulkanImage>(description);
	}

	Ref<ImageSampler> VulkanGraphicsDevice::CreateImageSampler(const ImageSamplerDescription& description)
	{
		return _resources.Create<VulkanImageSampler>(description);
	}

	void VulkanGraphicsDevice::ResetForNewFrame()
	{
		_cache->ResetForNextFrame();
	}

	void VulkanGraphicsDevice::PurgeUnusedResources()
	{
		_resources.PurgeUnused();

		//uint64 purgeCount = _resources.PurgeUnused();

		//if (purgeCount)
		//{
		//	CocoTrace("Purged {} graphics resources", purgeCount)
		//}
	}

	DeviceQueue* VulkanGraphicsDevice::GetQueue(DeviceQueue::Type queueType)
	{
		switch (queueType)
		{
		case DeviceQueue::Type::Graphics:
			return _graphicsQueue.get();
		case DeviceQueue::Type::Transfer:
			return _transferQueue.get();
		case DeviceQueue::Type::Compute:
			return _computeQueue.get();
		default:
			return nullptr;
		}
	}

	const DeviceQueue* VulkanGraphicsDevice::GetQueue(DeviceQueue::Type queueType) const
	{
		switch (queueType)
		{
		case DeviceQueue::Type::Graphics:
			return _graphicsQueue.get();
		case DeviceQueue::Type::Transfer:
			return _transferQueue.get();
		case DeviceQueue::Type::Compute:
			return _computeQueue.get();
		default:
			return nullptr;
		}
	}

	DeviceQueue* VulkanGraphicsDevice::GetOrCreatePresentQueue(VkSurfaceKHR surface)
	{
		if (_presentQueue)
			return _presentQueue;

		if (CheckQueuePresentSupport(surface, _graphicsQueue.get()))
			_presentQueue = _graphicsQueue.get();
		else if (CheckQueuePresentSupport(surface, _transferQueue.get()))
			_presentQueue = _transferQueue.get();
		else if (CheckQueuePresentSupport(surface, _computeQueue.get()))
			_presentQueue = _computeQueue.get();

		return _presentQueue;
	}

	bool VulkanGraphicsDevice::FindMemoryIndex(uint32 type, VkMemoryPropertyFlags memoryProperties, uint32& outIndex) const
	{
		VkPhysicalDeviceMemoryProperties deviceMemoryProperties{};
		vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &deviceMemoryProperties);

		for (uint32 i = 0; i < deviceMemoryProperties.memoryTypeCount; i++)
		{
			if (type & (1 << i) && (deviceMemoryProperties.memoryTypes[i].propertyFlags & memoryProperties) == memoryProperties)
			{
				outIndex = i;
				return true;
			}
		}

		return false;
	}

	void VulkanGraphicsDevice::WaitForQueueIdle(DeviceQueue::Type queueType) const
	{
		const DeviceQueue* queue = GetQueue(queueType);
		VkResult result = VK_ERROR_UNKNOWN;

		if (queue)
		{
			result = vkQueueWaitIdle(queue->Queue);
		}

		if (result != VK_SUCCESS)
			WaitForIdle();
	}

	PhysicalDeviceQueueFamilyInfo VulkanGraphicsDevice::GetQueueFamilyInfo(VkPhysicalDevice device)
	{
		PhysicalDeviceQueueFamilyInfo info{};

		uint32 queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
		std::vector <VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyProperties.data());

		int graphicsIndex = -1;
		int transferIndex = -1;
		int computeIndex = -1;
		int minimumTransferScore = Math::MaxValue<int>();

		for (uint32 i = 0; i < queueFamilyCount; i++)
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
			info.GraphicsQueueFamily = graphicsIndex;

		if (transferIndex != -1)
			info.TransferQueueFamily = transferIndex;

		if (computeIndex != -1)
			info.ComputeQueueFamily = computeIndex;

		return info;
	}

	int VulkanGraphicsDevice::CalculateDeviceScore(const VkPhysicalDevice& device, const GraphicsDeviceCreateParams& createParams)
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

	VkPhysicalDevice VulkanGraphicsDevice::PickPhysicalDevice(const VkInstance& instance, const GraphicsDeviceCreateParams& createParams)
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

		if (firstRank.Score == -1)
			throw std::exception("No suitable graphics device found");

		return firstRank.Device;
	}

	void VulkanGraphicsDevice::CreateLogicalDevice(const GraphicsDeviceCreateParams& createParams)
	{
		// Create unique queues
		PhysicalDeviceQueueFamilyInfo queueFamilies = GetQueueFamilyInfo(_physicalDevice);
		std::set<int> uniqueQueueFamilies;

		if (queueFamilies.GraphicsQueueFamily.has_value())
			uniqueQueueFamilies.insert(queueFamilies.GraphicsQueueFamily.value());

		if (queueFamilies.TransferQueueFamily.has_value())
			uniqueQueueFamilies.insert(queueFamilies.TransferQueueFamily.value());

		if (queueFamilies.ComputeQueueFamily.has_value())
			uniqueQueueFamilies.insert(queueFamilies.ComputeQueueFamily.value());

		const float queuePriorities = 1.0f; // TODO: individual queue priorities?

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

		for (const int& queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo& createInfo = queueCreateInfos.emplace_back();
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			createInfo.queueCount = 1; // TODO: multiple queues?
			createInfo.queueFamilyIndex = static_cast<uint32_t>(queueFamily);
			createInfo.pQueuePriorities = &queuePriorities;
		}

		std::vector<const char*> extensions;

		if (createParams.SupportsPresentation)
			extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

		VkPhysicalDeviceFeatures deviceFeatures{};

		// TODO: configurable features
		deviceFeatures.samplerAnisotropy = VK_TRUE;
		deviceFeatures.depthClamp = VK_TRUE;

		// Create the logical device
		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.ppEnabledExtensionNames = extensions.data();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.enabledLayerCount = 0;

		AssertVkSuccess(vkCreateDevice(_physicalDevice, &createInfo, GetAllocationCallbacks(), &_device))

		// Obtain queues from the device
		for (const int& queueFamily : uniqueQueueFamilies)
		{
			uint8 familyIndex = static_cast<uint8>(queueFamily);

			VkQueue queue;
			vkGetDeviceQueue(_device, familyIndex, 0, &queue);

			if (queueFamilies.GraphicsQueueFamily.has_value() && queueFamilies.GraphicsQueueFamily.value() == queueFamily)
			{
				_graphicsQueue = CreateUniqueRef<DeviceQueue>(this, DeviceQueue::Type::Graphics, familyIndex, queue);
			}

			if (queueFamilies.TransferQueueFamily.has_value() && queueFamilies.TransferQueueFamily.value() == queueFamily)
			{
				_transferQueue = CreateUniqueRef<DeviceQueue>(this, DeviceQueue::Type::Transfer, familyIndex, queue);
			}

			if (queueFamilies.ComputeQueueFamily.has_value() && queueFamilies.ComputeQueueFamily.value() == queueFamily)
			{
				_computeQueue = CreateUniqueRef<DeviceQueue>(this, DeviceQueue::Type::Compute, familyIndex, queue);
			}
		}
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
		_minUniformBufferAlignment = static_cast<uint32>(deviceProperties.limits.minUniformBufferOffsetAlignment);

		// Get the memory features of the physical device
		VkPhysicalDeviceMemoryProperties memoryProperties{};
		vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &memoryProperties);

		for (uint32 i = 0; i < memoryProperties.memoryTypeCount; i++)
		{
			if ((memoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0 &&
				(memoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0)
			{
				_memoryFeatures.SupportsHostVisibleLocalMemory = true;
				break;
			}
		}
	}

	bool VulkanGraphicsDevice::CheckQueuePresentSupport(VkSurfaceKHR surface, const DeviceQueue* queue)
	{
		if (!queue)
			return false;

		VkBool32 supported = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(_physicalDevice, queue->FamilyIndex, surface, &supported);

		return supported == VK_TRUE;
	}
}