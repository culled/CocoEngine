#pragma once

#include <Coco/Core/Defines.h>

namespace Coco::Rendering
{
	/// @brief An ID for a GraphicsResource
	using GraphicsResourceID = uint64;

	/// @brief A resource managed by a GraphicsDevice
	class GraphicsResource
	{
	public:
		/// @brief This resource's ID
		const GraphicsResourceID ID;

	public:
		virtual ~GraphicsResource() = default;

	protected:
		GraphicsResource(const GraphicsResourceID& id);
	};
}