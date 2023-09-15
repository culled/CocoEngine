#include "Renderpch.h"
#include "GraphicsDeviceResource.h"

namespace Coco::Rendering
{
	GraphicsDeviceResourceIDGenerator::GraphicsDeviceResourceIDGenerator() :
		CurrentID{0}
	{}

	GraphicsDeviceResourceID GraphicsDeviceResourceIDGenerator::operator()()
	{
		return CurrentID.fetch_add(1);
	}

	GraphicsDeviceResourceBase::GraphicsDeviceResourceBase(const GraphicsDeviceResourceID& id) :
		ID(id)
	{}
}