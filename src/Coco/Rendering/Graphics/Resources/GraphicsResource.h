#pragma once

#include <Coco/Rendering/RenderingResource.h>
#include <Coco/Rendering/RenderingService.h>

namespace Coco::Rendering
{
	template<typename DeviceType>
	class COCOAPI BaseGraphicsResource
	{
	protected:
		DeviceType* _device;

	public:
		template<typename ... Args>
		BaseGraphicsResource(Args&& ... args) :
			_device(static_cast<DeviceType*>(RenderingService::Get()->GetPlatform()->GetDevice()))
		{}
		virtual ~BaseGraphicsResource() = default;
	};

	template<typename DeviceType, typename BaseClass>
	class COCOAPI GraphicsResource : public BaseClass
	{
	protected:
		DeviceType* _device;

	public:
		template<typename ... Args>
		GraphicsResource(ResourceID id, const string& name, uint64_t lifetime, Args&& ... args) : 
			BaseClass(id, name, lifetime, std::forward<Args>(args)...), 
			_device(static_cast<DeviceType*>(RenderingService::Get()->GetPlatform()->GetDevice()))
		{}
		virtual ~GraphicsResource() = default;
	};
}