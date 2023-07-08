#include "CachedResource.h"

namespace Coco
{
	CachedResource::CachedResource(ResourceID referenceID, ResourceVersion referenceVersion) : 
		ReferenceID(referenceID),
		_referenceVersion(referenceVersion)
	{}
}
