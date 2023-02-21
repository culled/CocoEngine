#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Version.h>

#include "GraphicsResource.h"
#include "GraphicsPlatformTypes.h"

namespace Coco::Rendering
{
	/// <summary>
	/// Parameters for choosing and creating a graphics device
	/// </summary>
	struct COCOAPI GraphicsDeviceCreationParameters
	{
		/// <summary>
		/// The ideal type of rendering device to pick
		/// </summary>
		GraphicsDeviceType PreferredDeviceType = GraphicsDeviceType::Discrete;

		/// <summary>
		/// If true, the device will be able to present images
		/// </summary>
		bool SupportsPresentation = true;

		/// <summary>
		/// If true, the device will be required to support graphics operations
		/// </summary>
		bool RequireGraphicsCapability = true;

		/// <summary>
		/// If true, the device will be required to support compute operations
		/// </summary>
		bool RequireComputeCapability = true;

		/// <summary>
		/// If true, the device will be required to support memory transfer operations
		/// </summary>
		bool RequireTransferCapability = true;
	};

	/// <summary>
	/// A graphics device that can perform rendering-related operations
	/// </summary>
	class COCOAPI GraphicsDevice
	{
	public:
		virtual ~GraphicsDevice() = default;

		/// <summary>
		/// Gets the name of this device
		/// </summary>
		/// <returns>This device's name</returns>
		virtual string GetName() const = 0;

		/// <summary>
		/// Gets the type of this device 
		/// </summary>
		/// <returns>This device's type</returns>
		virtual GraphicsDeviceType GetType() const = 0;

		/// <summary>
		/// Gets the version of this device's driver
		/// </summary>
		/// <returns>This device's driver version</returns>
		virtual Version GetDriverVersion() const = 0;

		/// <summary>
		/// Gets the version of this device's API
		/// </summary>
		/// <returns>This device's API version</returns>
		virtual Version GetAPIVersion() const = 0;

		/// <summary>
		/// Creates a resource wrapper for the given raw resource and adds it to this device's list of managed resources
		/// </summary>
		/// <typeparam name="T">The type of resource being added</typeparam>
		/// <param name="resource">The raw resource</param>
		/// <returns>A managed resource</returns>
		template<typename T>
		ManagedGraphicsResource<T>* CreateAndAddRawResource(T* rawResource)
		{
			ManagedGraphicsResource<T> wrapper = new ManagedGraphicsResource<T>(this, rawResource);
			AddResource(wrapper);
			return wrapper;
		}

		/// <summary>
		/// Creates a GraphicsResource-derived type and adds it to this device's list of managed resources
		/// </summary>
		/// <typeparam name="T">The type of resource being created</typeparam>
		/// <typeparam name="...Args">The type of arguments to pass to the resource constructor</typeparam>
		/// <param name="...args">The arguments to pass to the resource's constructor</param>
		/// <returns>A managed resource</returns>
		template<typename T, typename ... Args, typename = std::enable_if_t<std::is_base_of_v<GraphicsResource, T>>>
		T* CreateAndAddResource(Args&& ... args)
		{
			T wrapper = new T(this, std::forward<Args>(args)...);
			AddResource(wrapper);
			return wrapper;
		}

		/// <summary>
		/// Adds a resource for this device to manage
		/// </summary>
		/// <param name="resource">The resource to add</param>
		virtual void AddResource(GraphicsResource* resource) = 0;

		/// <summary>
		/// Releases a managed resource without destroying it
		/// </summary>
		/// <param name="resource">The resource to release</param>
		virtual void ReleaseResource(GraphicsResource* resource) = 0;

		/// <summary>
		/// Destroys a managed resource
		/// </summary>
		/// <param name="resource">The resource to destroy</param>
		virtual void DestroyResource(GraphicsResource* resource) = 0;

		/// <summary>
		/// Blocks until this device is idle
		/// </summary>
		virtual void WaitForIdle() = 0;
	};
}

