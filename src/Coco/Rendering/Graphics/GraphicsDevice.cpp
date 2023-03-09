#include "GraphicsDevice.h"

namespace Coco::Rendering
{
	void GraphicsDevice::AddResource(Managed<GraphicsResource> resource)
	{
		Resources.Add(std::move(resource));
	}

	void GraphicsDevice::ReleaseResource(GraphicsResource* resource)
	{
		auto it = std::find_if(Resources.begin(), Resources.end(), [resource](const Managed<GraphicsResource>& other) {
			return resource == other.get();
			});

		if (it != Resources.end())
		{
			// Release the resource before removing it to prevent it from being destroyed
			it->release();
			Resources.Erase(it);
		}
	}

	void GraphicsDevice::DestroyResource(GraphicsResource* resource)
	{
		auto it = std::find_if(Resources.begin(), Resources.end(), [resource](const Managed<GraphicsResource>& other) {
			return resource == other.get();
			});

		// Erasing will cause the resource to be destroyed
		if (it != Resources.end())
		{
			Resources.Erase(it);
		}
	}
}