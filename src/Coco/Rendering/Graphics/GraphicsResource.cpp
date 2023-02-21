#include "GraphicsResource.h"

#include "GraphicsDevice.h"

namespace Coco::Rendering
{
	GraphicsResource::GraphicsResource(GraphicsDevice* owningDevice) :
		OwningDevice(owningDevice)
	{}
}