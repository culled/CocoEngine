#pragma once

#include <Coco/Core/Defines.h>

namespace Coco::Rendering
{
	/// @brief A fence that can be used to make the host wait for an async GPU operation to complete
	class GraphicsFence
	{
	public:
		virtual ~GraphicsFence() = default;

		/// @brief Gets if this fence has been signaled
		/// @return True if this fence has been signaled
		virtual bool IsSignaled() const = 0;

		/// @brief Resets this fence
		virtual void Reset() = 0;

		/// @brief Blocks until this fence is signaled or the timeout occurs
		/// @param timeoutNs The maximum timeout to wait for, in nanoseconds. Set to the max value to wait indefinitely
		virtual void Wait(uint64 timeoutNs) = 0;
	};
}