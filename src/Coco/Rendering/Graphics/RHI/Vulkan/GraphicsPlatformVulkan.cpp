#include "GraphicsPlatformVulkan.h"

#include <Coco/Rendering/RenderingService.h>
#include <Coco/Core/Application.h>
#include <Coco/Core/Engine.h>
#include "GraphicsPresenterVulkan.h"
#include "RenderContextVulkan.h"
#include "BufferVulkan.h"
#include "ImageVulkan.h"
#include "ImageSamplerVulkan.h"

#include "VulkanUtilities.h"

namespace Coco::Rendering::Vulkan
{
	const char* GraphicsPlatformVulkan::s_debugValidationLayerName = "VK_LAYER_KHRONOS_validation";

	static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {

		if (pUserData == nullptr)
			return VK_FALSE;

		Logging::Logger* logger = static_cast<Logging::Logger*>(pUserData);

		string message = FormattedString("Validation layer message:\n\t{}", pCallbackData->pMessage);

		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			LogError(logger, message);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			LogWarning(logger, message);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			LogInfo(logger, message);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			LogTrace(logger, message);
			break;
		default:
			break;
		}

		return VK_FALSE;
	}

	GraphicsPlatformVulkan::GraphicsPlatformVulkan(RenderingService* renderingService, const GraphicsPlatformCreationParameters& creationParams) :
		GraphicsPlatform(renderingService, creationParams), _deviceCreationParams(creationParams.DeviceCreateParams)
	{
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pEngineName = "Coco";
		appInfo.engineVersion = ToVkVersion(Version(1, 0, 0)); // TODO: get version
		appInfo.pApplicationName = creationParams.ApplicationName.c_str();
		appInfo.applicationVersion = ToVkVersion(Version(1, 0, 0)); // TODO: get version
		appInfo.apiVersion = VK_API_VERSION_1_2; // TODO: option to choose api version?

		List<const char*> requestedExtensionNames;
		List<string> platformExtensionNames;

		Engine::Get()->GetPlatform()->GetPlatformRenderingExtensions(static_cast<int>(GetRHI()), SupportsPresentation, platformExtensionNames);

		for (const string& extension : platformExtensionNames)
			requestedExtensionNames.Add(extension.c_str());

		if (SupportsPresentation)
			requestedExtensionNames.Add(VK_KHR_SURFACE_EXTENSION_NAME);

		List<const char*> requestedLayerNames;

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};

#if _DEBUG
		if (CheckValidationLayersSupport())
		{
			_usingValidationLayers = true;
		}
		else
		{
			LogWarning(GetLogger(), "Validation layers are not supported");
		}

		if (_usingValidationLayers)
		{
			requestedLayerNames.Add(s_debugValidationLayerName);
			requestedExtensionNames.Add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			debugCreateInfo = GetDebugCreateInfo();
		}
#endif

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(requestedExtensionNames.Count());
		createInfo.ppEnabledExtensionNames = requestedExtensionNames.Data();
		createInfo.enabledLayerCount = static_cast<uint32_t>(requestedLayerNames.Count());
		createInfo.ppEnabledLayerNames = requestedLayerNames.Data();

		if (_usingValidationLayers)
		{
			createInfo.pNext = &debugCreateInfo;
		}
		else
		{
			createInfo.pNext = nullptr;
		}

		AssertVkResult(vkCreateInstance(&createInfo, nullptr, &_instance));

		if (_usingValidationLayers)
			CreateDebugMessenger();

		_device = GraphicsDeviceVulkan::Create(*this, creationParams.DeviceCreateParams);

		LogTrace(GetLogger(), "Created Vulkan graphics platform");
	}

	GraphicsPlatformVulkan::~GraphicsPlatformVulkan()
	{
		_device.reset();

		if (_debugMessenger != nullptr)
		{
			DestroyDebugMessenger();
		}

		if (_instance != nullptr)
		{
			vkDestroyInstance(_instance, nullptr);
			_instance = nullptr;
		}

		LogTrace(GetLogger(), "Destroyed Vulkan graphics platform");
	}

	Logging::Logger* GraphicsPlatformVulkan::GetLogger() const noexcept
	{
		return RenderService->GetLogger();
	}

	void GraphicsPlatformVulkan::ResetDevice()
	{
		// TODO: handle how to reinitialize resources!
		LogInfo(GetLogger(), "Resetting graphics device...");
		_device = GraphicsDeviceVulkan::Create(*this, _deviceCreationParams);
		LogInfo(GetLogger(), "Graphics device reset");
	}

	Managed<GraphicsPresenter> GraphicsPlatformVulkan::CreatePresenter()
	{
		return CreateManaged<GraphicsPresenterVulkan>(_device.get());
	}

	Ref<Buffer> GraphicsPlatformVulkan::CreateBuffer(uint64_t size, BufferUsageFlags usageFlags, bool bindOnCreate)
	{
		return _device->CreateResource<BufferVulkan>(usageFlags, size, bindOnCreate);
	}

	Ref<Image> GraphicsPlatformVulkan::CreateImage(const ImageDescription& description)
	{
		return _device->CreateResource<ImageVulkan>(description);
	}

	Ref<ImageSampler> GraphicsPlatformVulkan::CreateImageSampler(FilterMode filterMode, RepeatMode repeatMode, uint maxAnisotropy)
	{
		return _device->CreateResource<ImageSamplerVulkan>(filterMode, repeatMode, maxAnisotropy);
	}

	bool GraphicsPlatformVulkan::CheckValidationLayersSupport() noexcept
	{
		try
		{
			uint32_t layerCount;
			vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

			List<VkLayerProperties> availableLayers(layerCount);
			vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.Data());

			for (const VkLayerProperties& layerProperties : availableLayers)
			{
				if (strcmp(&s_debugValidationLayerName[0], layerProperties.layerName) == 0)
				{
					return true;
				}
			}
		}
		catch(...)
		{ }

		return false;
	}

	VkDebugUtilsMessengerCreateInfoEXT GraphicsPlatformVulkan::GetDebugCreateInfo() const noexcept
	{
		VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = &VulkanDebugCallback;
		createInfo.pUserData = GetLogger();

		return createInfo;
	}

	bool GraphicsPlatformVulkan::CreateDebugMessenger() noexcept
	{
		PFN_vkCreateDebugUtilsMessengerEXT func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(_instance, "vkCreateDebugUtilsMessengerEXT"));

		if (func != nullptr)
		{
			const VkDebugUtilsMessengerCreateInfoEXT createInfo = GetDebugCreateInfo();
			return func(_instance, &createInfo, nullptr, &_debugMessenger) == VK_SUCCESS;
		}
		else
		{
			LogWarning(GetLogger(), "Could not find debug messenger creation function");
			return false;
		}
	}

	void GraphicsPlatformVulkan::DestroyDebugMessenger() noexcept
	{
		PFN_vkDestroyDebugUtilsMessengerEXT func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(_instance, "vkDestroyDebugUtilsMessengerEXT"));

		if (func != nullptr)
		{
			func(_instance, _debugMessenger, nullptr);
			_debugMessenger = nullptr;
		}
		else
		{
			LogWarning(GetLogger(), "Could not find debug messenger deletion function");
		}
	}
}