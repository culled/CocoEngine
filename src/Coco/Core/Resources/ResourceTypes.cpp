#include "Corepch.h"
#include "ResourceTypes.h"

#include "../Math/Math.h"

namespace Coco
{
	const ResourceID ResourceID::Invalid = ResourceID(Math::MaxValue<uint64>());

	std::hash<string> _sStringHasher;

	ResourceID::ResourceID() :
		ResourceID(Invalid)
	{}

	ResourceID::ResourceID(const string& idStr) :
		_id(idStr),
		_hash(_sStringHasher(idStr))
	{}

	ResourceID::ResourceID(const uint64& hash) :
		_id(std::to_string(hash)),
		_hash(hash)
	{}
}