#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Version.h>
#include "GraphicsResource.h"

#include "GraphicsPlatformTypes.h"

namespace Coco::Rendering
{
	//template<class>
	//class GraphicsResourceRef;

	class IGraphicsResource;

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
	protected:
		/// <summary>
		/// The list of resources this device manages
		/// </summary>
		List<Managed<IGraphicsResource>> Resources;

	protected:
		GraphicsDevice() = default;

	public:
		virtual ~GraphicsDevice() = default;

		GraphicsDevice(const GraphicsDevice&) = delete;
		GraphicsDevice(GraphicsDevice&&) = delete;

		GraphicsDevice& operator=(const GraphicsDevice&) = delete;
		GraphicsDevice& operator=(GraphicsDevice&&) = delete;

		/// <summary>
		/// Gets the name of this device
		/// </summary>
		/// <returns>This device's name</returns>
		virtual string GetName() const noexcept = 0;

		/// <summary>
		/// Gets the type of this device 
		/// </summary>
		/// <returns>This device's type</returns>
		virtual GraphicsDeviceType GetType() const noexcept = 0;

		/// <summary>
		/// Gets the version of this device's driver
		/// </summary>
		/// <returns>This device's driver version</returns>
		virtual Version GetDriverVersion() const noexcept = 0;

		/// <summary>
		/// Gets the version of this device's API
		/// </summary>
		/// <returns>This device's API version</returns>
		virtual Version GetAPIVersion() const noexcept = 0;

		/// <summary>
		/// Blocks until this device is idle
		/// </summary>
		virtual void WaitForIdle() noexcept = 0;

		/// <summary>
		/// Creates a graphics resource, adds it to this device's list of managed resources, and returns a handle to it
		/// </summary>
		/// <typeparam name="T">The type of resource being created</typeparam>
		/// <typeparam name="...Args">The type of arguments to pass to the resource constructor</typeparam>
		/// <param name="...args">The arguments to pass to the resource's constructor</param>
		/// <returns>A handle to the resource</returns>
		template<typename ObjectT, typename ... Args, std::enable_if_t<std::is_base_of<IGraphicsResource, ObjectT>::value, bool> = true>
		GraphicsResourceRef<ObjectT> CreateResource(Args&& ... args)
		{
			Resources.Add(CreateManaged<ObjectT>(this, std::forward<Args>(args)...));
			Managed<IGraphicsResource>& resource = Resources[Resources.Count() - 1];
			return GraphicsResourceRef<ObjectT>(static_cast<ObjectT*>(resource.get()), [&](ObjectT* resource) 
				{ 
					this->DestroyResource(resource); 
				});
		}

		/// <summary>
		/// Destroys a managed graphics resource
		/// </summary>
		/// <param name="resource">The resource to destroy</param>
		void DestroyResource(const IGraphicsResource* resource);
	};
}

