#pragma once

#include "../RenderService.h"

namespace Coco::Rendering
{
	/// @brief A resource that requires a GraphicsDevice
	/// @tparam DeviceType The type of device
	template<class DeviceType>
	class GraphicsDeviceResource
	{
	protected:
		DeviceType* _device;

		GraphicsDeviceResource()
		{
			RenderService* service = RenderService::Get();
			Assert(service != nullptr)
			Assert(service->GetDevice() != nullptr)

			_device = static_cast<DeviceType*>(RenderService::Get()->GetDevice());
		}
	};
}