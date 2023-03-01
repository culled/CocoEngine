#pragma once

#include <Coco/Core/Core.h>

namespace Coco::Rendering
{
	class GraphicsDevice;

	/// <summary>
	/// A generic resource that a GraphicsDevice manages
	/// </summary>
	class COCOAPI GraphicsResource
	{
	protected:
		/// <summary>
		/// The device that owns this resource
		/// </summary>
		GraphicsDevice* OwningDevice;

	public:
		GraphicsResource(GraphicsDevice* owningDevice);
		virtual ~GraphicsResource() = default;

		/// <summary>
		/// Gets the device that owns this resource
		/// </summary>
		/// <returns>The owning GraphicsDevice</returns>
		GraphicsDevice* GetOwningDevice() const { return OwningDevice; }

		/// <summary>
		/// Gets the raw resource
		/// </summary>
		/// <returns>A pointer to the raw resource</returns>
		virtual void* GetResource() const = 0;
	};

	/// <summary>
	/// A resource that a GraphicsDevice manages
	/// </summary>
	template<typename T>
	class COCOAPI ManagedGraphicsResource : public GraphicsResource
	{
	protected:
		/// <summary>
		/// The managed resource
		/// </summary>
		Managed<T> Resource;

	public:
		ManagedGraphicsResource(GraphicsDevice* owningDevice, T* resource) : GraphicsResource(owningDevice), Resource(resource) {}
		virtual ~ManagedGraphicsResource() override { Resource.reset(); }

		virtual void* GetResource() const override { return Resource.get(); }

		/// <summary>
		/// Gets the raw resource
		/// </summary>
		/// <returns>A pointer to the raw resource</returns>
		T* GetResource() const { return Resource.get(); }

		/// <summary>
		/// Releases the raw resource without destroying it
		/// </summary>
		/// <returns>A pointer to the raw resource</returns>
		T* ReleaseResource() { return Resource.release(); }
	};
}