#pragma once
#include "../../../GraphicsResource.h"

namespace Coco::Rendering::Vulkan
{
	class CachedVulkanResource
	{
	public:
		const uint64 ID;

	public:
		virtual ~CachedVulkanResource() = default;

		/// @brief Marks this resource as used
		virtual void Use();

		/// @brief Determines if this resource is stale and can be purged
		/// @param staleThreshold The amount of time without use that a resource becomes stale
		/// @return True if this pass can be purged
		virtual bool IsStale(double staleThreshold) const;

	protected:
		double _lastUseTime;

	protected:
		CachedVulkanResource(const uint64& id);
	};
}