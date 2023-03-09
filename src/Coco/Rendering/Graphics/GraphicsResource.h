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

	protected:
		GraphicsResource(GraphicsDevice* owningDevice);

	public:
		virtual ~GraphicsResource() = default;

		/// <summary>
		/// Gets the device that owns this resource
		/// </summary>
		/// <returns>The owning GraphicsDevice</returns>
		GraphicsDevice* GetOwningDevice() const { return OwningDevice; }
	};
}