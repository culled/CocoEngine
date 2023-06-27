#pragma once

#include <Coco/Core/Core.h>
#include "ResourceTypes.h"

namespace Coco
{
	class COCOAPI CachedResource
	{
	protected:
		ResourceID _originalID;
		ResourceVersion _originalVersion;

	public:
		CachedResource(ResourceID originalID, ResourceVersion originalVersion);
		virtual ~CachedResource() = default;

		virtual bool IsValid() const = 0;
		virtual bool NeedsUpdate() const = 0;

		ResourceID GetOriginalID() const { return _originalID; }
		ResourceVersion GetOriginalVersion() const { return _originalVersion; }

	protected:
		void UpdateOriginalVersion(ResourceVersion version) { _originalVersion = version; }
	};
}
