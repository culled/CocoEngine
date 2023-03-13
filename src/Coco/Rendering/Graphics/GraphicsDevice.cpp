#include "GraphicsDevice.h"

#include "GraphicsResource.h"

namespace Coco::Rendering
{
	void GraphicsDevice::DestroyResource(const IGraphicsResource* resource)
	{
		WaitForIdle();

		auto it = std::find_if(Resources.begin(), Resources.end(), [resource](const Managed<IGraphicsResource>& other) {
			return resource == other.get();
			});

		// Erasing will cause the resource to be destroyed
		if (it != Resources.end())
		{
			Resources.Erase(it);
		}
	}
}