#pragma once

#include <Coco/Core/Core.h>

#include "GraphicsResource.h"

namespace Coco::Rendering
{
	class GraphicsDevice;

	/// <summary>
	/// A fence that can be used to make the host wait for an async GPU operation to complete
	/// </summary>
	class COCOAPI GraphicsFence : public GraphicsResource
	{
	protected:
		GraphicsFence(GraphicsDevice* owningDevice) : GraphicsResource(owningDevice) {}

	public:
		virtual ~GraphicsFence() = default;

		/// <summary>
		/// Gets if this fence has been signalled
		/// </summary>
		/// <returns>True if this fence has been signalled</returns>
		virtual bool IsSignalled() const = 0;

		/// <summary>
		/// Resets this fence
		/// </summary>
		virtual void Reset() = 0;

		/// <summary>
		/// Waits on this fence
		/// </summary>
		/// <param name="timeoutNs">The maximum timeout to wait for, in nanoseconds</param>
		virtual void Wait(unsigned long long timeoutNs) = 0;
	};
}