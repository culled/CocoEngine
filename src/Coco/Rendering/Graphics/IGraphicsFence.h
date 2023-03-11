#pragma once

#include <Coco/Core/Core.h>
#include "GraphicsResource.h"

namespace Coco::Rendering
{
	/// <summary>
	/// A fence that can be used to make the host wait for an async GPU operation to complete
	/// </summary>
	class COCOAPI IGraphicsFence : public IGraphicsResource
	{
	protected:
		IGraphicsFence() = default;

	public:
		virtual ~IGraphicsFence() = default;

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
		virtual void Wait(uint64_t timeoutNs) = 0;
	};
}