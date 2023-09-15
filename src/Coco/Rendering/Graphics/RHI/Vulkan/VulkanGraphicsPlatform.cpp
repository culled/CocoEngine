#include "Renderpch.h"
#include "VulkanGraphicsPlatform.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanUtils.h"
#include "../../../RenderingPlatform.h"
#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	const char* VulkanGraphicsPlatform::sVulkanRHIName = "Vulkan";
	const char* VulkanGraphicsPlatform::_sDebugValidationLayerName = "VK_LAYER_KHRONOS_validation";

	VulkanGraphicsPlatform::VulkanGraphicsPlatform(const GraphicsPlatformCreateParams& createParams) :
		_vulkanInstance(nullptr),
		_debugMessenger(nullptr),
		_usingValidationLayers(false),
		_supportsPresentation(createParams.PresentationSupport)
	{
		CreateVulkanInstance(createParams);

		CocoTrace("Created VulkanGraphicsPlatform")
	}

	VulkanGraphicsPlatform::~VulkanGraphicsPlatform()
	{
		if (_debugMessenger)
		{
			DestroyDebugMessenger();
			_debugMessenger = nullptr;
		}

		if (_vulkanInstance)
		{
			vkDestroyInstance(_vulkanInstance, nullptr);
			_vulkanInstance = nullptr;
		}

		CocoTrace("Destroyed VulkanGraphicsPlatform")
	}

	UniqueRef<GraphicsDevice> VulkanGraphicsPlatform::CreateDevice(const GraphicsDeviceCreateParams& createParams)
	{
		GraphicsDeviceCreateParams createParamsCopy = createParams;

		// Overwrite presentation support if the platform doesn't support it
		createParamsCopy.SupportsPresentation = _supportsPresentation && createParams.SupportsPresentation;

		return VulkanGraphicsDevice::Create(_vulkanInstance, createParamsCopy);
	}

	bool VulkanGraphicsPlatform::CheckValidationLayerSupport()
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const VkLayerProperties& layerProperties : availableLayers)
		{
			if (strcmp(_sDebugValidationLayerName, layerProperties.layerName) == 0)
			{
				return true;
			}
		}

		return false;
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {

		static const char* messageFormat = "Validation layer message:\n\t{}";

		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		{
			CocoError(messageFormat, pCallbackData->pMessage)
			break;
		}
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		{
			CocoWarn(messageFormat, pCallbackData->pMessage)
			break;
		}
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		{
			CocoInfo(messageFormat, pCallbackData->pMessage)
			break;
		}
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		{
			CocoTrace(messageFormat, pCallbackData->pMessage)
			break;
		}
		default:
			break;
		}

		return VK_FALSE;
	}

	VkDebugUtilsMessengerCreateInfoEXT VulkanGraphicsPlatform::GetDebugCreateInfo()
	{
		VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = &VulkanDebugCallback;
		createInfo.pUserData = nullptr;

		return createInfo;
	}

	bool VulkanGraphicsPlatform::CreateDebugMessenger()
	{
		PFN_vkCreateDebugUtilsMessengerEXT func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(_vulkanInstance, "vkCreateDebugUtilsMessengerEXT"));

		if (func)
		{
			const VkDebugUtilsMessengerCreateInfoEXT createInfo = GetDebugCreateInfo();
			return func(_vulkanInstance, &createInfo, nullptr, &_debugMessenger) == VK_SUCCESS;
		}
		else
		{
			CocoWarn("Could not find debug messenger creation function")
			return false;
		}
	}

	void VulkanGraphicsPlatform::DestroyDebugMessenger()
	{
		PFN_vkDestroyDebugUtilsMessengerEXT func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(_vulkanInstance, "vkDestroyDebugUtilsMessengerEXT"));

		if (func)
		{
			func(_vulkanInstance, _debugMessenger, nullptr);
		}
		else
		{
			CocoWarn("Could not find debug messenger deletion function")
		}
	}

	void VulkanGraphicsPlatform::CreateVulkanInstance(const GraphicsPlatformCreateParams& createParams)
	{
		const Engine* engine = Engine::cGet();

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pEngineName = "Coco";
		appInfo.engineVersion = ToVkVersion(engine->GetVersion());
		appInfo.pApplicationName = createParams.App.GetName();
		appInfo.applicationVersion = ToVkVersion(createParams.App.GetVersion());
		appInfo.apiVersion = VK_API_VERSION_1_2; // TODO: option to choose api version?

		std::vector<const char*> extensions;
		std::vector<const char*> layers;

		if (createParams.PresentationSupport)
			extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

		if (const RenderingPlatform* platform = dynamic_cast<const RenderingPlatform*>(engine->GetPlatform()))
		{
			platform->GetPlatformRenderingExtensions(sVulkanRHIName, createParams.PresentationSupport, extensions);
		}
		else
		{
			throw std::exception("Platform is not a RenderingPlatform");
		}

		for (const auto& ext : createParams.RenderingExtensions)
		{
			extensions.push_back(ext);
		}

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};

#if _DEBUG
		if (CheckValidationLayerSupport())
		{
			_usingValidationLayers = true;

			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			layers.push_back(_sDebugValidationLayerName);
			debugCreateInfo = GetDebugCreateInfo();
		}
		else
		{
			CocoWarn("Validation layers are not supported")
		}
#endif

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();
		createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
		createInfo.ppEnabledLayerNames = layers.data();
		createInfo.pNext = _usingValidationLayers ? &debugCreateInfo : nullptr;

		AssertVkSuccess(vkCreateInstance(&createInfo, nullptr, &_vulkanInstance))

		if (_usingValidationLayers && !CreateDebugMessenger())
		{
			CocoError("Failed to create debug messenger")
			_debugMessenger = nullptr;
		}
	}
}