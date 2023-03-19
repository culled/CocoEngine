#include "GraphicsDevice.h"

#include "GraphicsResource.h"
#include <Coco/Rendering/RenderingService.h>
#include <Coco/Core/Logging/Logger.h>

namespace Coco::Rendering
{
	void GraphicsDevice::PurgeUnusedResources() noexcept
	{
		auto it = Resources.begin();
		uint64_t purgeCount = 0;

		while (it != Resources.end())
		{
			// A use count of 1 means only the device is referencing the resource
			if ((*it).use_count() <= 1)
			{
				it = Resources.EraseAndGetNext(it);
				purgeCount++;
			}
			else
				it++;
		}

		if (purgeCount > 0)
		{
			RenderingService* service = RenderingService::Get();
			LogTrace(service->GetLogger(), FormattedString("Purged {} resources", purgeCount));
		}


		OnPurgedResources.Invoke();
	}
}