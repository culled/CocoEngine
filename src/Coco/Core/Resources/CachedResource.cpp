#include "CachedResource.h"

namespace Coco
{
	CachedResource::CachedResource(const ResourceID& referenceID, ResourceVersion referenceVersion) : 
		ReferenceID(referenceID),
		_referenceVersion(referenceVersion)
	{}
}
