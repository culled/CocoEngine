#include "GraphicsDevice.h"

#include <Coco/Core/Logging/Logger.h>
#include "Resources/GraphicsResource.h"

namespace Coco::Rendering
{
	void GraphicsDevice::PurgeUnusedResources() noexcept
	{
		auto it = Resources.begin();
		uint64_t purgeCount = 0;

		while (it != Resources.end())
		{
			if ((*it).GetUseCount() <= 0)
			{
				it = Resources.EraseAndGetNext(it);
				purgeCount++;
			}
			else
				it++;
		}

		if (purgeCount > 0)
			LogTrace(GetLogger(), FormattedString("Purged {} unused graphics resources ({} in use)", purgeCount, Resources.Count()));

		OnPurgeUnusedResources();

		try
		{
			OnPurgedResources.Invoke();
		}
		catch(...)
		{ }
	}
}