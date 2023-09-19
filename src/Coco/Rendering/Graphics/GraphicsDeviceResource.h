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

	class GraphicsDeviceResourceBase
	{
	public:
		/// @brief The ID of this resource
		const GraphicsDeviceResourceID ID;

	public:
		GraphicsDeviceResourceBase(const GraphicsDeviceResourceID& id);

		virtual ~GraphicsDeviceResourceBase() = default;
	};

	/// @brief A resource that requires a GraphicsDevice
	/// @tparam DeviceType The type of device
	template<class DeviceType>
	class GraphicsDeviceResource : public GraphicsDeviceResourceBase
	{
	protected:
		DeviceType* _device;

		GraphicsDeviceResource(const GraphicsDeviceResourceID& id) :
			GraphicsDeviceResourceBase(id)
		{
			RenderService* service = RenderService::Get();
			Assert(service != nullptr)
			Assert(service->GetDevice() != nullptr)

			_device = static_cast<DeviceType*>(RenderService::Get()->GetDevice());
		}
	};
}