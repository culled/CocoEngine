#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Version.h>
#include <Coco/Core/Types/ManagedRef.h>
#include <Coco/Core/Events/Event.h>
#include "Resources/GraphicsResource.h"
#include "GraphicsPlatformTypes.h"

namespace Coco::Logging
{
	class Logger;
}

namespace Coco::Rendering
{
	class IGraphicsResource;

	/// <summary>
	/// Memory features for a GraphicsDevice
	/// </summary>
	struct COCOAPI GraphicsDeviceMemoryFeatures
	{
		/// <summary>
		/// If true, this device support host visible local memory
		/// </summary>
		bool SupportsLocalHostBufferMemory = false;
	};

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
		Event<> OnPurgedResources;

	protected:
		/// <summary>
		/// The list of resources this device manages
		/// </summary>
		List<ManagedRef<IGraphicsResource>> Resources;

	protected:
		GraphicsDevice() = default;

	public:
		virtual ~GraphicsDevice() = default;

		GraphicsDevice(const GraphicsDevice&) = delete;
		GraphicsDevice(GraphicsDevice&&) = delete;

		GraphicsDevice& operator=(const GraphicsDevice&) = delete;
		GraphicsDevice& operator=(GraphicsDevice&&) = delete;

		/// <summary>
		/// Gets this graphics devices's logger
		/// </summary>
		/// <returns>This graphics platform's logger</returns>
		virtual Logging::Logger* GetLogger() const noexcept = 0;

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
		/// Gets memory features for the device
		/// </summary>
		/// <returns>The device's memory features</returns>
		virtual const GraphicsDeviceMemoryFeatures& GetMemoryFeatures() const noexcept = 0;

		/// <summary>
		/// Gets the minimum alignment for a buffer that this device supports
		/// </summary>
		/// <returns>The minimum buffer alignment for this device</returns>
		virtual uint GetMinimumBufferAlignment() const noexcept = 0;

		/// <summary>
		/// Creates a graphics resource, adds it to this device's list of managed resources, and returns a handle to it
		/// </summary>
		/// <typeparam name="T">The type of resource being created</typeparam>
		/// <typeparam name="...Args">The type of arguments to pass to the resource constructor</typeparam>
		/// <param name="...args">The arguments to pass to the resource's constructor</param>
		/// <returns>A handle to the resource</returns>
		template<typename ObjectT, typename ... Args>
		WeakManagedRef<ObjectT> CreateResource(Args&& ... args)
		{
			static_assert(std::is_base_of_v<IGraphicsResource, ObjectT>, "The resource must be derived from IGraphicsResource");

			ManagedRef<IGraphicsResource> ref(new ObjectT(this, std::forward<Args>(args)...));

			//Ref<ObjectT> ref = CreateRef<ObjectT>(this, std::forward<Args>(args)...);
			Resources.Add(std::move(ref));
			return WeakManagedRef<ObjectT>(Resources.Last());
		}

		/// <summary>
		/// Purges unused graphics resources
		/// </summary>
		void PurgeUnusedResources() noexcept;

	protected:
		/// <summary>
		/// Called when this device purges resources
		/// </summary>
		virtual void OnPurgeUnusedResources() noexcept {}
	};
}

