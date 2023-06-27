#include "GraphicsDevice.h"

#include <Coco/Core/Logging/Logger.h>

namespace Coco::Rendering
{
	GraphicsDevice::GraphicsDevice()
	{
		Resources = CreateManagedRef<ResourceLibrary>();
	}

	void GraphicsDevice::PurgeResource(const Ref<Resource>& resource, bool forcePurge)
	{
		// We add the test for use count to account for the caller probably still owing the resource.
		// If the caller is the only one owning it (use count == 2), then force purge it
		Resources->PurgeResource(resource->GetID(), forcePurge || resource.GetUseCount() <= 2);
	}

	void GraphicsDevice::PurgeUnusedResources() noexcept
	{
		uint64_t purgeCount = Resources->PurgeStaleResources();

		if (purgeCount > 0)
			LogTrace(GetLogger(), FormattedString("Purged {} unused graphics resources ({} in use)", purgeCount, Resources->GetResourceCount()));

		try
		{
			OnPurgedResources.Invoke();
		}
		catch (...)
		{
		}
	}
}