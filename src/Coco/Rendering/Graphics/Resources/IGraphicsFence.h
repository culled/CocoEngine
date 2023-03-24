#pragma once

#include "GraphicsResource.h"

namespace Coco::Rendering
{
	/// @brief A fence that can be used to make the host wait for an async GPU operation to complete
	class COCOAPI IGraphicsFence : public IGraphicsResource
	{
	protected:
		IGraphicsFence() = default;

	public:
		virtual ~IGraphicsFence() = default;

		IGraphicsFence(const IGraphicsFence&) = delete;
		IGraphicsFence(IGraphicsFence&&) = delete;

		IGraphicsFence& operator=(const IGraphicsFence&) = delete;
		IGraphicsFence& operator=(IGraphicsFence&&) = delete;

		/// @brief Gets if this fence has been signalled
		/// @return True if this fence has been signalled
		virtual bool IsSignalled() const noexcept = 0;

		/// @brief Resets this fence
		virtual void Reset() = 0;

		/// @brief Waits on this fence
		/// @param timeoutNs The maximum timeout to wait for, in nanoseconds
		virtual void Wait(uint64_t timeoutNs) = 0;
	};
}