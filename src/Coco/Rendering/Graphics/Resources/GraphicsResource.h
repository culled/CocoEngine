#pragma once

#include <Coco/Rendering/RenderingResource.h>
#include <Coco/Rendering/RenderingService.h>

namespace Coco::Rendering
{
	/// @brief A resource for a GraphicsDevice
	/// @tparam DeviceType The type of device
	template<typename DeviceType>
	class COCOAPI BaseGraphicsResource
	{
	protected:
		/// @brief The graphics device
		DeviceType* _device;

	public:
		template<typename ... Args>
		BaseGraphicsResource(Args&& ... args) :
			_device(static_cast<DeviceType*>(RenderingService::Get()->GetPlatform()->GetDevice()))
		{}
		virtual ~BaseGraphicsResource() = default;
	};

	/// @brief A resource for a GraphicsDevice that is derived from a resource
	/// @tparam DeviceType The type of device
	/// @tparam BaseClass The base class type
	template<typename DeviceType, typename BaseClass>
	class COCOAPI GraphicsResource : public BaseClass
	{
	protected:
		/// @brief The graphics device
		DeviceType* _device;

	public:
		template<typename ... Args>
		GraphicsResource(const ResourceID& id, const string& name, Args&& ... args) : 
			BaseClass(id, name, std::forward<Args>(args)...), 
			_device(static_cast<DeviceType*>(RenderingService::Get()->GetPlatform()->GetDevice()))
		{}
		virtual ~GraphicsResource() = default;
	};
}