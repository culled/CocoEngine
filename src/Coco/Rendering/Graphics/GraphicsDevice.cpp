#include "GraphicsDevice.h"

#include "GraphicsResource.h"

namespace Coco::Rendering
{
	void GraphicsDevice::DestroyResource(const IGraphicsResource* resource) noexcept
	{
		WaitForIdle();

		auto it = Resources.Find([resource](const Managed<IGraphicsResource>& other) noexcept {
			return resource == other.get();
			});

		// Erasing will cause the resource to be destroyed
		if (it != Resources.end())
		{
			Resources.Remove(it);
		}
	}
}