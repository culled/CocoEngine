#include "GraphicsDevice.h"

#include <Coco/Core/Logging/Logger.h>

namespace Coco::Rendering
{
	GraphicsDevice::GraphicsDevice()
	{
		_resources = CreateManagedRef<ResourceLibrary>("Graphics");
	}

	void GraphicsDevice::PurgeResource(const Ref<Resource>& resource, bool forcePurge)
	{
		// We add the test for use count to account for the caller probably still owing the resource.
		// If the caller is the only one owning it (use count == 3; referenced by this function, the caller, and the device), then force purge it
		_resources->PurgeResource(resource->ID, forcePurge || resource.GetUseCount() <= 3);
	}
}