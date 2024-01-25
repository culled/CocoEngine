#include "Corepch.h"
#include "ResourceSerializerFactory.h"
#include "ResourceSerializer.h"
#include "../Types/CoreExceptions.h"

namespace Coco
{
	UniqueRef<ResourceSerializer> ResourceSerializerFactory::Create(const string& resourceTypename)
	{
		auto& map = GetMap();
		if (!map.contains(resourceTypename))
			throw NotFoundException(FormatString("Could not find a registered ResourceSerializer for the type \"{}\"", resourceTypename));

		return map.at(resourceTypename)();
	}

	std::unordered_map<string, ResourceSerializerFactory::ResourceSerializerCreateFunc>& ResourceSerializerFactory::GetMap()
	{
		static std::unordered_map<string, ResourceSerializerFactory::ResourceSerializerCreateFunc> _sCreateFunctions;
		return _sCreateFunctions;
	}
}