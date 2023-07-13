#include "GraphicsPlatformVulkan.h"

#include <Coco/Rendering/RenderingService.h>
#include <Coco/Rendering/IRenderingPlatform.h>
#include <Coco/Core/Application.h>
#include <Coco/Core/Engine.h>
#include "Resources/GraphicsPresenterVulkan.h"
#include "Resources/RenderContextVulkan.h"
#include "Resources/BufferVulkan.h"
#include "Resources/ImageVulkan.h"
#include "Resources/ImageSamplerVulkan.h"
#include "GraphicsDeviceVulkan.h"

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

	GraphicsPlatformVulkan::GraphicsPlatformVulkan(const GraphicsPlatformCreationParameters& creationParams) :
		GraphicsPlatform(creationParams), 
		_deviceCreationParams(creationParams.DeviceCreateParams)
	{
		List<string> platformExtensionNames;

		Platform::IRenderingPlatform* renderingPlatform = dynamic_cast<Platform::IRenderingPlatform*>(Engine::Get()->GetPlatform());
		if (renderingPlatform == nullptr)
			throw RenderingException("Platform does not support rendering");

		renderingPlatform->GetRenderingExtensions(static_cast<int>(GetRHI()), SupportsPresentation, platformExtensionNames);

		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pEngineName = "Coco";
		appInfo.engineVersion = ToVkVersion(Version(1, 0, 0)); // TODO: get version
		appInfo.pApplicationName = creationParams.ApplicationName.c_str();
		appInfo.applicationVersion = ToVkVersion(Version(1, 0, 0)); // TODO: get version
		appInfo.apiVersion = VK_API_VERSION_1_2; // TODO: option to choose api version?

		List<const char*> requestedExtensionNames;

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
		_device.Reset();

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

	Logging::Logger* GraphicsPlatformVulkan::GetLogger() noexcept { return RenderingService::Get()->GetLogger(); }

	GraphicsDevice* GraphicsPlatformVulkan::GetDevice() noexcept { return _device.Get(); }

	//void GraphicsPlatformVulkan::ResetDevice()
	//{
	//	// TODO: handle how to reinitialize resources!
	//	LogInfo(GetLogger(), "Resetting graphics device...");
	//	_device = GraphicsDeviceVulkan::Create(*this, _deviceCreationParams);
	//	LogInfo(GetLogger(), "Graphics device reset");
	//}

	Ref<GraphicsPresenter> GraphicsPlatformVulkan::CreatePresenter(const string& name)
	{
		return _device->CreateResource<GraphicsPresenterVulkan>(name);
	}

	Ref<Buffer> GraphicsPlatformVulkan::CreateBuffer(const string& name, uint64_t size, BufferUsageFlags usageFlags, bool bindOnCreate)
	{
		return _device->CreateResource<BufferVulkan>(name, usageFlags, size, bindOnCreate);
	}

	Ref<Image> GraphicsPlatformVulkan::CreateImage(const string& name, const ImageDescription& description)
	{
		return _device->CreateResource<ImageVulkan>(name, description);
	}

	Ref<ImageSampler> GraphicsPlatformVulkan::CreateImageSampler(const string& name, const ImageSamplerProperties& properties)
	{
		return _device->CreateResource<ImageSamplerVulkan>(name, properties);
	}

	Matrix4x4 GraphicsPlatformVulkan::CreateOrthographicProjection(double left, double right, double top, double bottom, double nearClip, double farClip) noexcept
	{
		Matrix4x4 ortho = Matrix4x4::Identity;

		const double w = 2.0 / (right - left);
		const double h = 2.0 / (top - bottom);
		const double a = 1.0 / (farClip - nearClip);
		const double b = -a * nearClip;

		// This creates an orthographic matrix for Vulkan/DirectX with the directions: right = X, up = -Y, forward = Z
		ortho.Data[Matrix4x4::m11] = w;
		ortho.Data[Matrix4x4::m22] = h;
		ortho.Data[Matrix4x4::m33] = a;

		ortho.Data[Matrix4x4::m14] = -(right + left) / (right - left);
		ortho.Data[Matrix4x4::m24] = -(top + bottom) / (top - bottom);
		ortho.Data[Matrix4x4::m34] = b;

		return GraphicsPlatform::CocoViewToRenderView * ortho;
	}

	Matrix4x4 GraphicsPlatformVulkan::CreateOrthographicProjection(double size, double aspectRatio, double nearClip, double farClip) noexcept
	{
		const double halfHeight = size * 0.5;
		const double halfWidth = halfHeight * aspectRatio;

		return CreateOrthographicProjection(-halfWidth, halfWidth, halfHeight, -halfHeight, nearClip, farClip);
	}

	Matrix4x4 GraphicsPlatformVulkan::CreatePerspectiveProjection(double fieldOfViewRadians, double aspectRatio, double nearClip, double farClip) noexcept
	{
		Matrix4x4 perspective;

		const double h = 1.0 / Math::Tan(fieldOfViewRadians * 0.5);
		const double w = h / aspectRatio;
		const double a = farClip / (farClip - nearClip);
		const double b = (-nearClip * farClip) / (farClip - nearClip);

		// This creates a projection matrix for Vulkan/DirectX with the directions: right = X, up = -Y, forward = Z
		perspective.Data[Matrix4x4::m11] = w;
		perspective.Data[Matrix4x4::m22] = h;
		perspective.Data[Matrix4x4::m33] = a;
		perspective.Data[Matrix4x4::m44] = 0.0;

		perspective.Data[Matrix4x4::m34] = b;
		perspective.Data[Matrix4x4::m43] = 1.0;

		return GraphicsPlatform::CocoViewToRenderView * perspective;
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

	VkDebugUtilsMessengerCreateInfoEXT GraphicsPlatformVulkan::GetDebugCreateInfo() noexcept
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