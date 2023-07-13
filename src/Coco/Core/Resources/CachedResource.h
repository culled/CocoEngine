#pragma once

#include <Coco/Core/Core.h>
#include "ResourceTypes.h"

namespace Coco
{
	/// @brief A cached resource that tracks outside resources
	class COCOAPI CachedResource
	{
	public:
		/// @brief The ID that this cached resource references
		const ResourceID ReferenceID;

	protected:
		ResourceVersion _referenceVersion;

	protected:
		CachedResource(const ResourceID& referenceID, ResourceVersion referenceVersion);

	public:
		virtual ~CachedResource() = default;

		/// @brief Gets the last version of the original resources that this cached resource was updated for
		/// @return The last version of the original resources that this cached resource was updated for
		ResourceVersion GetReferenceVersion() const noexcept { return _referenceVersion; }

		/// @brief Gets if this cached resource is still valid
		/// @return True if this cached resource is still valid
		virtual bool IsValid() const = 0;

		/// @brief Gets if this cached resource should be updated
		/// @return True if this cached resource should be updated
		virtual bool NeedsUpdate() const = 0;

	protected:
		/// @brief Updates the referenced version
		/// @param version The new version
		void UpdateReferenceVersion(ResourceVersion version) noexcept { _referenceVersion = version; }
	};
}
