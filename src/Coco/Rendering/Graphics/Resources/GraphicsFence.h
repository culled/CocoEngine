#pragma once

#include <Coco/Rendering/RenderingResource.h>

namespace Coco::Rendering
{
	/// @brief A fence that can be used to make the host wait for an async GPU operation to complete
	class COCOAPI GraphicsFence : public RenderingResource
	{
	protected:
		GraphicsFence(const ResourceID& id, const string& name) : RenderingResource(id, name) {}

	public:
		virtual ~GraphicsFence() = default;

		GraphicsFence(const GraphicsFence&) = delete;
		GraphicsFence(GraphicsFence&&) = delete;

		GraphicsFence& operator=(const GraphicsFence&) = delete;
		GraphicsFence& operator=(GraphicsFence&&) = delete;

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