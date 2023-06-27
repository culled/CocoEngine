#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Core/Events/Event.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Version.h>
#include <Coco/Core/Resources/ResourceLibrary.h>
#include "GraphicsDeviceTypes.h"
#include "../RenderingResource.h"

namespace Coco::Logging
{
	class Logger;
}

namespace Coco::Rendering
{
	/// @brief A graphics device that can perform rendering-related operations
	class COCOAPI GraphicsDevice
	{
	public:
		/// @brief Invoked when this device purges unused graphics resources
		Event<> OnPurgedResources;

	protected:
		/// @brief The list of resources this device manages
		ManagedRef<ResourceLibrary> Resources;

	protected:
		GraphicsDevice();

	public:
		virtual ~GraphicsDevice() = default;

		GraphicsDevice(const GraphicsDevice&) = delete;
		GraphicsDevice(GraphicsDevice&&) = delete;

		GraphicsDevice& operator=(const GraphicsDevice&) = delete;
		GraphicsDevice& operator=(GraphicsDevice&&) = delete;

		/// @brief Gets this graphics devices's logger
		/// @return This graphics platform's logger
		virtual Logging::Logger* GetLogger() noexcept = 0;

		/// @brief Gets the name of this device
		/// @return This device's name
		virtual string GetName() const noexcept = 0;

		/// @brief Gets the type of this device
		/// @return This device's type
		virtual GraphicsDeviceType GetType() const noexcept = 0;

		/// @brief Gets the version of this device's driver
		/// @return This device's driver version
		virtual Version GetDriverVersion() const noexcept = 0;

		/// @brief Gets the version of this device's API
		/// @return This device's API version
		virtual Version GetAPIVersion() const noexcept = 0;
	
		/// @brief Gets memory features for the device
		/// @return The device's memory features
		virtual const GraphicsDeviceMemoryFeatures& GetMemoryFeatures() const noexcept = 0;

		/// @brief Gets the minimum alignment for a buffer that this device supports
		/// @return The minimum buffer alignment for this device
		virtual uint GetMinimumBufferAlignment() const noexcept = 0;

		/// @brief Blocks until this device has finished all async operations
		virtual void WaitForIdle() noexcept = 0;

		/// @brief Creates a graphics resource, adds it to this device's list of managed resources, and returns a handle to it
		/// @tparam ResourceType 
		/// @tparam ...Args 
		/// @param ...args The arguments to pass to the resource's constructor
		/// @return A handle to the resource
		template<typename ResourceType, typename ... Args>
		Ref<ResourceType> CreateResource(Args&& ... args)
		{
			static_assert(std::is_base_of_v<RenderingResource, ResourceType>, "The resource must be derived from RenderingResource");
		
			return Resources->CreateResource<ResourceType>("", RenderingService::DefaultGraphicsResourceTickLifetime, std::forward<Args>(args)...);
		}

		void PurgeResource(const Ref<Resource>& resource, bool forcePurge = false);

		/// @brief Purges unused graphics resources
		virtual void PurgeUnusedResources() noexcept;
	};
}

