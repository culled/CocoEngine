#include "FileResource.h"

namespace Coco
{
	FileResource::FileResource(ResourceID id, const string& name, uint64_t tickLifetime, const string& filePath) noexcept : Resource(id, name, tickLifetime),
		_filePath(filePath), _isLoaded(false)
	{
	}
}
