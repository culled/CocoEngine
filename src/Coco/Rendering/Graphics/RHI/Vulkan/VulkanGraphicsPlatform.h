#pragma once

#include "../../GraphicsPlatform.h"
#include "../../GraphicsPlatformTypes.h"
#include "VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
	/// @brief Vulkan implementation of a GraphicsPlatform
	class VulkanGraphicsPlatform : 
		public GraphicsPlatform
	{
	public:
		static const char* sVulkanRHIName;

	private:
		static const char* _sDebugValidationLayerName;

		VkInstance _vulkanInstance;
		VkDebugUtilsMessengerEXT _debugMessenger;
		bool _usingValidationLayers;
		bool _supportsPresentation;

	public:
		VulkanGraphicsPlatform(const GraphicsPlatformCreateParams& createParams, uint32 apiVersion, bool useValidationLayers);
		~VulkanGraphicsPlatform();

		const char* GetName() const final { return sVulkanRHIName; }
		UniqueRef<GraphicsDevice> CreateDevice(const GraphicsDeviceCreateParams& createParams) final;
		Matrix4x4 CreateOrthographicProjection(double left, double right, double bottom, double top, double nearClip, double farClip) final;
		Matrix4x4 CreateOrthographicProjection(double size, double aspectRatio, double nearClip, double farClip) final;
		Matrix4x4 CreatePerspectiveProjection(double verticalFOVRadians, double aspectRatio, double nearClip, double farClip) final;

		/// @brief Gets the Vulkan instance
		/// @return The Vulkan instance
		VkInstance GetVulkanInstance() const { return _vulkanInstance; }

		/// @brief Gets the Vulkan platform allocation callbacks
		/// @return The platform allocation callbacks
		VkAllocationCallbacks* GetAllocationCallbacks() { return nullptr; } // TODO: allocation callbacks

	private:
		/// @brief Checks if validation layers are supported
		/// @return True if validation layers are supported
		static bool CheckValidationLayerSupport();

		/// @brief Gets a struct for debugging Vulkan operations
		/// @return Create info for a debugger
		VkDebugUtilsMessengerCreateInfoEXT GetDebugCreateInfo();

		/// @brief Tries to create a debug messenger to log validation messages
		/// @return True if the messenger was created
		bool CreateDebugMessenger();

		/// @brief Destroys the current debug messenger
		void DestroyDebugMessenger();

		/// @brief Creates the Vulkan instance
		/// @param createParams Platform creation parameters
		/// @param apiVersion The version of the Vulkan api to use
		void CreateVulkanInstance(const GraphicsPlatformCreateParams& createParams, uint32 apiVersion);
	};
}

