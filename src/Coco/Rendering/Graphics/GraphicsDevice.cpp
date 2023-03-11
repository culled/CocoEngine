#include "GraphicsDevice.h"

namespace Coco::Rendering
{
	void GraphicsDevice::DestroyResource(const IGraphicsResource* resource)
	{
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