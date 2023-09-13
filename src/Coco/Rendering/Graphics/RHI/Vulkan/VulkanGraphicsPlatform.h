#pragma once

#include "../../GraphicsPlatform.h"
#include "VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
	/// @brief Vulkan implementation of a GraphicsPlatform
	class VulkanGraphicsPlatform : public GraphicsPlatform
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
		VulkanGraphicsPlatform(const GraphicsPlatformCreateParams& createParams);
		~VulkanGraphicsPlatform();

		const char* GetName() const final { return sVulkanRHIName; }
		UniqueRef<GraphicsDevice> CreateDevice(const GraphicsDeviceCreateParams& createParams) final;
		UniqueRef<GraphicsPresenter> CreatePresenter() final;

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
		void CreateVulkanInstance(const GraphicsPlatformCreateParams& createParams);
	};
}

