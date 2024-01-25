#include "Renderpch.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanGraphicsPlatform.h"
#include "VulkanGraphicsPlatformFactory.h"
#include "VulkanPresenter.h"
#include "VulkanBuffer.h"
#include "VulkanImage.h"
#include "VulkanImageSampler.h"
#include "VulkanRenderContext.h"

#include <Coco/Core/Engine.h>
#include "VulkanUtils.h"

namespace Coco::Rendering::Vulkan
{
	VkPhysicalDeviceQueueFamilyInfo::VkPhysicalDeviceQueueFamilyInfo() :
		GraphicsQueueFamily(),
		TransferQueueFamily(),
		ComputeQueueFamily()
	{}

	VkPhysicalDeviceQueueFamilyInfo VkPhysicalDeviceQueueFamilyInfo::GetInfo(VkPhysicalDevice physicalDevice)
	{
		uint32 queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
		std::vector <VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

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

		VkPhysicalDeviceQueueFamilyInfo info{};

		if (graphicsIndex != -1)
			info.GraphicsQueueFamily = static_cast<uint32>(graphicsIndex);

		if (transferIndex != -1)
			info.TransferQueueFamily = static_cast<uint32>(transferIndex);

		if (computeIndex != -1)
			info.ComputeQueueFamily = static_cast<uint32>(computeIndex);

		return info;
	}

	VulkanGraphicsDeviceFeatures::VulkanGraphicsDeviceFeatures() :
		GraphicsDeviceFeatures(),
		MaxPushConstantSize(0)
	{}

	std::atomic_uint64_t VulkanGraphicsDevice::_sNextResourceID = 0;
	const double VulkanGraphicsDevice::ResourcePurgePeriod = 1.0;
	const double VulkanGraphicsDevice::StaleResourceThreshold = 1.5;
	const int VulkanGraphicsDevice::ResourcePurgeTickPriority = -200000;

	VulkanGraphicsDevice::VulkanGraphicsDevice(const VulkanGraphicsPlatform& platform, const VulkanGraphicsDeviceCreateParams& createParams, VkPhysicalDevice physicalDevice) :
		_platform(platform),
		_physicalDevice(physicalDevice),
		_features(GetDeviceFeatures(physicalDevice)),
		_purgeTickListener(CreateManagedRef<TickListener>(this, &VulkanGraphicsDevice::HandlePurgeTickListener, ResourcePurgeTickPriority)),
		_graphicsQueue(nullptr),
		_transferQueue(nullptr),
		_computeQueue(nullptr),
		_presentQueue(nullptr)
	{
		_purgeTickListener->SetTickPeriod(ResourcePurgePeriod);
		MainLoop::Get()->AddTickListener(_purgeTickListener);

		CreateDevice(createParams);

		_cache = CreateUniqueRef<VulkanGraphicsDeviceCache>(*this);

		CocoInfo("Using Vulkan on {} - Driver version {}, API version {}",
			_features.Name,
			_features.DriverVersion.ToString(),
			platform.GetAPIVersion().ToString()
		)

		CocoTrace("Created VulkanGraphicsDevice")
	}

	VulkanGraphicsDevice::~VulkanGraphicsDevice()
	{
		WaitForIdle();

		MainLoop::Get()->RemoveTickListener(_purgeTickListener);

		_cache.reset();

		if (_resources.size() > 0)
		{
			CocoTrace("Destroying {} GraphicsResource(s)", _resources.size())
			_resources.clear();
		}

		_presentQueue = nullptr;
		_graphicsQueue.reset();
		_transferQueue.reset();
		_computeQueue.reset();

		DestroyDevice();

		CocoTrace("Destroyed VulkanGraphicsDevice")
	}

	uint32 VulkanGraphicsDevice::GetDataTypeAlignment(BufferDataType type) const
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
		case BufferDataType::Matrix4x4:
			return BufferFloatSize * 16;
		default:
			return 0;
		}
	}

	Ref<Presenter> VulkanGraphicsDevice::CreatePresenter()
	{
		return CreateResource<VulkanPresenter>();
	}

	Ref<Buffer> VulkanGraphicsDevice::CreateBuffer(uint64 size, BufferUsageFlags usageFlags)
	{
		return CreateResource<VulkanBuffer>(size, usageFlags);
	}

	Ref<Image> VulkanGraphicsDevice::CreateImage(const ImageDescription& description)
	{
		return CreateResource<VulkanImage>(description);
	}

	Ref<ImageSampler> VulkanGraphicsDevice::CreateImageSampler(const ImageSamplerDescription& description)
	{
		return CreateResource<VulkanImageSampler>(description);
	}

	bool VulkanGraphicsDevice::TryReleaseResource(const GraphicsResourceID& resourceID)
	{
		auto it = _resources.find(resourceID);

		if (it == _resources.end())
			return false;

		if (it->second.GetUseCount() > 2)
			return false;

		ManagedRef<GraphicsResource> resource = std::move(it->second);
		_resources.erase(resourceID);
		resource.Invalidate();
		return true;
	}

	void VulkanGraphicsDevice::WaitForIdle()
	{
		AssertVkSuccess(vkDeviceWaitIdle(_device))
	}

	bool VulkanGraphicsDevice::PickPhysicalDevice(VkInstance instance, const VulkanGraphicsDeviceCreateParams& createParams, VkPhysicalDevice& outDevice)
	{
		using DeviceScorePair = std::pair<VkPhysicalDevice, int>;

		uint32_t deviceCount;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		if (deviceCount == 0)
			return false;

		std::vector<DeviceScorePair> deviceScores;

		std::transform(devices.begin(), devices.end(),
			std::back_inserter(deviceScores),
			[createParams](const VkPhysicalDevice& device)
			{
				return std::make_pair(device, CalculateDeviceScore(device, createParams));
			}
		);

		std::sort(deviceScores.begin(), deviceScores.end(),
			[](const DeviceScorePair& a, const DeviceScorePair& b)
			{
				return a.second > b.second;
			});

		const DeviceScorePair& firstRank = deviceScores.front();

		if (firstRank.second == -1)
			return false;

		outDevice = firstRank.first;
		return true;
	}

	VkInstance VulkanGraphicsDevice::GetVulkanInstance() const
	{
		return _platform.GetVulkanInstance();
	}

	const VkAllocationCallbacks* VulkanGraphicsDevice::GetAllocationCallbacks() const
	{
		return _platform.GetAllocationCallbacks();
	}

	VulkanQueue* VulkanGraphicsDevice::GetQueue(VulkanQueueType queueType)
	{
		switch (queueType)
		{
		case VulkanQueueType::Graphics:
			return _graphicsQueue.get();
		case VulkanQueueType::Transfer:
			return _transferQueue.get();
		case VulkanQueueType::Compute:
			return _computeQueue.get();
		case VulkanQueueType::Present:
			return _presentQueue;
		default:
			return nullptr;
		}
	}

	VulkanQueue* VulkanGraphicsDevice::GetOrCreatePresentQueue(VkSurfaceKHR surface)
	{
		if (_presentQueue)
			return _presentQueue;

		if (CheckPresentQueueSupport(surface, _graphicsQueue.get()))
			_presentQueue = _graphicsQueue.get();
		else if (CheckPresentQueueSupport(surface, _transferQueue.get()))
			_presentQueue = _transferQueue.get();
		else if (CheckPresentQueueSupport(surface, _computeQueue.get()))
			_presentQueue = _computeQueue.get();

		return _presentQueue;
	}

	Ref<VulkanRenderContext> VulkanGraphicsDevice::CreateRenderContext()
	{
		return CreateResource<VulkanRenderContext>();
	}

	SharedRef<RenderFrame> VulkanGraphicsDevice::StartNewRenderFrame()
	{
		_currentRenderFrame = _cache->GetRenderFramePool().Get();
		return _currentRenderFrame;
	}

	void VulkanGraphicsDevice::EndRenderFrame()
	{
		if (!_currentRenderFrame)
			return;

		_currentRenderFrame->SubmitRenderTasks();
		_currentRenderFrame.reset();
	}

	VulkanGraphicsDeviceFeatures VulkanGraphicsDevice::GetDeviceFeatures(VkPhysicalDevice physicalDevice)
	{
		// Get basic device info
		VkPhysicalDeviceProperties deviceProperties{};
		vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

		VulkanGraphicsDeviceFeatures features{};

		features.Name = string(&deviceProperties.deviceName[0]);
		features.Type = ToGraphicsDeviceType(deviceProperties.deviceType);
		features.DriverVersion = ToVersion(deviceProperties.driverVersion);

		VkSampleCountFlags maxSamples = deviceProperties.limits.framebufferColorSampleCounts & deviceProperties.limits.framebufferDepthSampleCounts;
		features.MaximumMSAASamples = ToMSAASamples(maxSamples);

		features.MaxImageWidth = deviceProperties.limits.maxImageDimension1D;
		features.MaxImageHeight = deviceProperties.limits.maxImageDimension2D;
		features.MaxImageDepth = deviceProperties.limits.maxImageDimension3D;
		features.MinimumBufferAlignment = static_cast<uint32>(deviceProperties.limits.minUniformBufferOffsetAlignment);
		features.MaxAnisotropicLevel = static_cast<uint32>(deviceProperties.limits.maxSamplerAnisotropy);
		features.MaxPushConstantSize = deviceProperties.limits.maxPushConstantsSize;

		return features;
	}

	int VulkanGraphicsDevice::CalculateDeviceScore(const VkPhysicalDevice& device, const VulkanGraphicsDeviceCreateParams& createParams)
	{
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);

		int score = 0;
		bool isMissingCriticalRequirement = false;

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
		VkPhysicalDeviceQueueFamilyInfo queueFamilies = VkPhysicalDeviceQueueFamilyInfo::GetInfo(device);

		// Check for a graphics queue
		if (queueFamilies.GraphicsQueueFamily.has_value())
			score++;
		else
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

		if (createParams.PresentationSupport)
			requiredExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

		for (const string& requiredExtension : requiredExtensions)
		{
			if (!std::any_of(deviceExtensions.begin(), deviceExtensions.end(),
				[requiredExtension, score](const VkExtensionProperties& properties)
				{
					return strcmp(&properties.extensionName[0], requiredExtension.c_str()) == 0;
				}
			))
			{
				isMissingCriticalRequirement = true;
				break;
			}
		}

		return isMissingCriticalRequirement ? -1 : score;
	}

	GraphicsResourceID VulkanGraphicsDevice::GetNewResourceID()
	{
		return _sNextResourceID++;
	}

	void VulkanGraphicsDevice::CreateDevice(const VulkanGraphicsDeviceCreateParams& createParams)
	{
		// Create unique queues
		VkPhysicalDeviceQueueFamilyInfo queueFamilies = VkPhysicalDeviceQueueFamilyInfo::GetInfo(_physicalDevice);
		std::set<uint32> uniqueQueueFamilies;

		if (queueFamilies.GraphicsQueueFamily.has_value())
			uniqueQueueFamilies.insert(queueFamilies.GraphicsQueueFamily.value());

		if (queueFamilies.TransferQueueFamily.has_value())
			uniqueQueueFamilies.insert(queueFamilies.TransferQueueFamily.value());

		if (queueFamilies.ComputeQueueFamily.has_value())
			uniqueQueueFamilies.insert(queueFamilies.ComputeQueueFamily.value());

		const float queuePriorities = 1.0f; // TODO: individual queue priorities?

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

		for (const uint32& queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo& createInfo = queueCreateInfos.emplace_back(VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO);
			createInfo.queueCount = 1; // TODO: multiple queues?
			createInfo.queueFamilyIndex = queueFamily;
			createInfo.pQueuePriorities = &queuePriorities;
		}

		// Fill out device extensions
		std::vector<const char*> extensions;

		if (createParams.PresentationSupport)
			extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

		VkPhysicalDeviceFeatures deviceFeatures{};

		// TODO: more configurable features
		deviceFeatures.samplerAnisotropy = createParams.EnableAnisotropicSampling;
		deviceFeatures.depthClamp = createParams.EnableDepthClamping;

		deviceFeatures.fillModeNonSolid = createParams.EnableWireframeDrawing;
		_features.SupportsWireframe = createParams.EnableWireframeDrawing;

		// Create the logical device
		VkDeviceCreateInfo createInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.ppEnabledExtensionNames = extensions.data();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.enabledLayerCount = 0;

		AssertVkSuccess(vkCreateDevice(_physicalDevice, &createInfo, GetAllocationCallbacks(), &_device))

		VmaAllocatorCreateInfo allocatorCreateInfo{};
		allocatorCreateInfo.vulkanApiVersion = ToVkVersion(_platform.GetAPIVersion());
		allocatorCreateInfo.physicalDevice = _physicalDevice;
		allocatorCreateInfo.device = _device;
		allocatorCreateInfo.instance = _platform.GetVulkanInstance();

		AssertVkSuccess(vmaCreateAllocator(&allocatorCreateInfo, &_allocator))

		// Obtain queues from the device
		for (const uint32& queueFamily : uniqueQueueFamilies)
		{
			VkQueue queue;
			vkGetDeviceQueue(_device, queueFamily, 0, &queue);
		
			if (queueFamilies.GraphicsQueueFamily.has_value() && queueFamilies.GraphicsQueueFamily.value() == queueFamily)
			{
				_graphicsQueue = CreateUniqueRef<VulkanQueue>(*this, queueFamily, queue, VulkanQueueType::Graphics);
			}
		
			if (queueFamilies.TransferQueueFamily.has_value() && queueFamilies.TransferQueueFamily.value() == queueFamily)
			{
				_transferQueue = CreateUniqueRef<VulkanQueue>(*this, queueFamily, queue, VulkanQueueType::Transfer);
			}
		
			if (queueFamilies.ComputeQueueFamily.has_value() && queueFamilies.ComputeQueueFamily.value() == queueFamily)
			{
				_computeQueue = CreateUniqueRef<VulkanQueue>(*this, queueFamily, queue, VulkanQueueType::Compute);
			}
		}
	}

	void VulkanGraphicsDevice::DestroyDevice()
	{
		if (_allocator)
		{
			vmaDestroyAllocator(_allocator);
			_allocator = nullptr;
		}

		if (_device)
		{
			vkDestroyDevice(_device, GetAllocationCallbacks());
			_device = nullptr;
		}
	}

	bool VulkanGraphicsDevice::CheckPresentQueueSupport(VkSurfaceKHR surface, const VulkanQueue* queue)
	{
		if (!queue)
			return false;

		VkBool32 supported = VK_FALSE;
		AssertVkSuccess(vkGetPhysicalDeviceSurfaceSupportKHR(_physicalDevice, queue->GetFamilyIndex(), surface, &supported))

		return supported == VK_TRUE;
	}

	void VulkanGraphicsDevice::HandlePurgeTickListener(const TickInfo& tickInfo)
	{
		PurgeStaleResources();
	}

	void VulkanGraphicsDevice::PurgeStaleResources()
	{
		uint64 resourcesPurged = std::erase_if(_resources,
			[](const auto& kvp)
			{
				return kvp.second.GetUseCount() == 1;
			});

		if (resourcesPurged > 0)
		{
			CocoTrace("Purged {} unused graphics resources", resourcesPurged)
		}
	}
}