#include "Renderpch.h"
#include "VulkanGraphicsPlatform.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanGraphicsPlatformFactory.h"
#include "../../../RenderingPlatform.h"
#include "../../../RenderingExceptions.h"

#include <Coco/Core/Engine.h>

#include "VulkanUtils.h"

namespace Coco::Rendering::Vulkan
{
	const string VulkanGraphicsPlatform::Name = "Vulkan";
	const char* DebugValidationLayerName = "VK_LAYER_KHRONOS_validation";

	VulkanGraphicsPlatform::VulkanGraphicsPlatform(const VulkanGraphicsPlatformCreateParams& createParams) :
		_usingValidationLayers(false),
		_deviceCreateParams(createParams.DeviceCreateParams),
		_debugMessenger(nullptr)
	{
		if (!createParams.PresentationSupport)
		{
			_deviceCreateParams.PresentationSupport = false;
			CocoInfo("Presentation support is disabled in the GraphicsPlatform - presentation support will be disabled for GraphicsDevices")
		}

		CreateVulkanInstance(createParams);

		CocoTrace("Created VulkanGraphicsPlatform using Vulkan {}", _apiVersion.ToString())
	}

	VulkanGraphicsPlatform::~VulkanGraphicsPlatform()
	{
		DestroyVulkanInstance();

		CocoTrace("Destroyed VulkanGraphicsPlatform")
	}

	Matrix4x4 VulkanGraphicsPlatform::CreateOrthographicProjection(double left, double right, double bottom, double top, double nearClip, double farClip)
	{
		const double w = 2.0 / (right - left);
		const double h = 2.0 / (top - bottom);
		const double a = 1.0 / (nearClip - farClip);

		// This creates an orthographic matrix to transform Coco view coordinates to Vulkan with the directions: right = X, up = -Y, forward = Z
		Matrix4x4 ortho;
		ortho.Data[Matrix4x4::m11] = w;
		ortho.Data[Matrix4x4::m22] = -h;
		ortho.Data[Matrix4x4::m33] = a;

		ortho.Data[Matrix4x4::m14] = -(right + left) / (right - left);
		ortho.Data[Matrix4x4::m24] = (top + bottom) / (top - bottom);
		ortho.Data[Matrix4x4::m34] = nearClip * a;

		ortho.Data[Matrix4x4::m44] = 1.0;

		return ortho;
	}

	Matrix4x4 VulkanGraphicsPlatform::CreateOrthographicProjection(double size, double aspectRatio, double nearClip, double farClip)
	{
		const double halfSize = size / 2.0;

		return CreateOrthographicProjection(-halfSize * aspectRatio, halfSize * aspectRatio, -halfSize, halfSize, nearClip, farClip);
	}

	Matrix4x4 VulkanGraphicsPlatform::CreatePerspectiveProjection(double verticalFOVRadians, double aspectRatio, double nearClip, double farClip)
	{
		const double h = 1.0 / Math::Tan(verticalFOVRadians * 0.5);
		const double w = h / aspectRatio;
		const double a = farClip / (farClip - nearClip);
		const double b = -nearClip * a;

		// This creates a projection matrix to transform Coco view coordinates to Vulkan with the directions: right = X, up = -Y, forward = Z
		Matrix4x4 perspective;
		perspective.Data[Matrix4x4::m11] = w;
		perspective.Data[Matrix4x4::m22] = -h;
		perspective.Data[Matrix4x4::m33] = -a;
		perspective.Data[Matrix4x4::m44] = 0.0;

		perspective.Data[Matrix4x4::m34] = b;
		perspective.Data[Matrix4x4::m43] = -1.0;

		return perspective;
	}

	UniqueRef<GraphicsDevice> VulkanGraphicsPlatform::CreateDevice() const
	{
		VkPhysicalDevice physicalDevice = nullptr;
		if (!VulkanGraphicsDevice::PickPhysicalDevice(_instance, _deviceCreateParams, physicalDevice))
		{
			throw RenderingInitException("Could not find a suitable graphics device");
		}

		return CreateUniqueRef<VulkanGraphicsDevice>(*this, _deviceCreateParams, physicalDevice);
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		const char* messageFormat = "Validation layer message:\n\t{}";

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
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		{
			CocoTrace(messageFormat, pCallbackData->pMessage)
			break;
		}
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		default:
			CocoInfo(messageFormat, pCallbackData->pMessage)
			break;
		}

		return VK_FALSE;
	}

	VkDebugUtilsMessengerCreateInfoEXT VulkanGraphicsPlatform::GetDebugCreateInfo()
	{
		VkDebugUtilsMessengerCreateInfoEXT createInfo{ VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = &VulkanDebugCallback;
		createInfo.pUserData = nullptr;

		return createInfo;
	}

	bool VulkanGraphicsPlatform::CheckValidationLayerSupport()
	{
		uint32 layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		return std::any_of(availableLayers.begin(), availableLayers.end(),
			[](const VkLayerProperties& layerProperties) 
			{ 
				return strcmp(DebugValidationLayerName, layerProperties.layerName) == 0;
			});
	}

	uint32 VulkanGraphicsPlatform::PickAPIVersion(const Version& version)
	{
		uint32 maxVersion;
		vkEnumerateInstanceVersion(&maxVersion);

		uint32 vkVersion = ToVkVersion(version);
		return Math::Min(maxVersion, vkVersion);
	}

	void VulkanGraphicsPlatform::CreateVulkanInstance(const VulkanGraphicsPlatformCreateParams& createParams)
	{
		const Engine* engine = Engine::cGet();

		const RenderingPlatform* platform = dynamic_cast<const RenderingPlatform*>(&engine->GetPlatform());
		CocoAssert(platform, "Platform does not support rendering")

		VkApplicationInfo appInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
		appInfo.pEngineName = "Coco";
		appInfo.engineVersion = ToVkVersion(engine->GetVersion());
		appInfo.pApplicationName = createParams.App.GetName();
		appInfo.applicationVersion = ToVkVersion(createParams.App.GetVersion());
		appInfo.apiVersion = PickAPIVersion(createParams.APIVersion);

		_apiVersion = ToVersion(appInfo.apiVersion);

		// Fill out the instance extensions
		std::vector<string> extensions;
		std::copy(createParams.RenderingExtensions.begin(), createParams.RenderingExtensions.end(), std::back_inserter(extensions));
		platform->GetPlatformRenderingExtensions(Name, createParams.PresentationSupport, extensions);

		if (createParams.PresentationSupport)
			extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

		std::vector<const char*> layers;
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		_usingValidationLayers = createParams.UseValidationLayers && CheckValidationLayerSupport();

		if (_usingValidationLayers)
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			layers.push_back(DebugValidationLayerName);
			debugCreateInfo = GetDebugCreateInfo();
		}
		else if(createParams.UseValidationLayers)
		{
			CocoWarn("Validation layers are not supported")
		}

		std::vector<const char*> extensionsCStr;
		std::transform(extensions.begin(), extensions.end(),
			std::back_inserter(extensionsCStr),
			[](const string& s)
			{
				return s.c_str();
			});

		VkInstanceCreateInfo createInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensionsCStr.size());
		createInfo.ppEnabledExtensionNames = extensionsCStr.data();
		createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
		createInfo.ppEnabledLayerNames = layers.data();
		createInfo.pNext = _usingValidationLayers ? &debugCreateInfo : nullptr;

		AssertVkSuccess(vkCreateInstance(&createInfo, GetAllocationCallbacks(), &_instance))

		if (_usingValidationLayers && !CreateDebugMessenger())
		{
			CocoError("Failed to create debug messenger")
			_debugMessenger = nullptr;
		}
	}

	void VulkanGraphicsPlatform::DestroyVulkanInstance()
	{
		if (_debugMessenger)
		{
			DestroyDebugMessenger();
		}

		if (_instance)
		{
			vkDestroyInstance(_instance, GetAllocationCallbacks());
			_instance = nullptr;
		}
	}

	bool VulkanGraphicsPlatform::CreateDebugMessenger()
	{
		PFN_vkCreateDebugUtilsMessengerEXT func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(_instance, "vkCreateDebugUtilsMessengerEXT"));

		if (func)
		{
			const VkDebugUtilsMessengerCreateInfoEXT createInfo = GetDebugCreateInfo();
			return func(_instance, &createInfo, GetAllocationCallbacks(), &_debugMessenger) == VK_SUCCESS;
		}
		else
		{
			CocoWarn("Could not find debug messenger creation function")
			return false;
		}
	}

	void VulkanGraphicsPlatform::DestroyDebugMessenger()
	{
		PFN_vkDestroyDebugUtilsMessengerEXT func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(_instance, "vkDestroyDebugUtilsMessengerEXT"));

		if (func)
		{
			func(_instance, _debugMessenger, GetAllocationCallbacks());
			_debugMessenger = nullptr;
		}
		else
		{
			CocoWarn("Could not find debug messenger deletion function")
		}
	}
}