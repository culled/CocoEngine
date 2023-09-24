#pragma once

#include <Coco/Core/Defines.h>
#include "../RenderService.h"
#include <Coco/Core/Resources/ResourceLibrary.h>

namespace Coco::Rendering
{
	using GraphicsDeviceResourceID = uint64;

	/// @brief A generator for GraphicsDeviceResourceIDs
	struct GraphicsDeviceResourceIDGenerator
	{
		std::atomic<GraphicsDeviceResourceID> CurrentID;

		GraphicsDeviceResourceIDGenerator();
		GraphicsDeviceResourceID operator()();
	};

	/// @brief Base class for all resources owned by a GraphicsDevice
	class GraphicsDeviceResourceBase
	{
	public:
		/// @brief The ID of this resource
		const GraphicsDeviceResourceID ID;

	public:
		GraphicsDeviceResourceBase(const GraphicsDeviceResourceID& id);

		virtual ~GraphicsDeviceResourceBase() = default;
	};

	/// @brief A resource that is owned by a GraphicsDevice
	/// @tparam DeviceType The type of device
	template<class DeviceType>
	class GraphicsDeviceResource : public GraphicsDeviceResourceBase
	{
	protected:
		DeviceType& _device;

		GraphicsDeviceResource(const GraphicsDeviceResourceID& id) :
			GraphicsDeviceResourceBase(id),
			_device(static_cast<DeviceType&>(RenderService::Get()->GetDevice()))
		{}
	};
}