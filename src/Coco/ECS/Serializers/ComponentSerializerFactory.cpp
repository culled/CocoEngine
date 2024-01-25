#include "ECSpch.h"
#include "ComponentSerializerFactory.h"
#include "ComponentSerializer.h"

namespace Coco::ECS
{
	UniqueRef<BaseComponentSerializer> ComponentSerializerFactory::Create(const string& componentTypename)
	{
		auto& map = GetMap();
		if (!map.contains(componentTypename))
			throw NotFoundException(FormatString("Could not find a registered ComponentSerializer for the type \"{}\"", componentTypename));

		return map.at(componentTypename)();
	}

	bool ComponentSerializerFactory::Has(const string& componentTypename)
	{
		return GetMap().contains(componentTypename);
	}

	std::unordered_map<string, ComponentSerializerFactory::ComponentSerializerCreateFunc>& ComponentSerializerFactory::GetMap()
	{
		static std::unordered_map<string, ComponentSerializerFactory::ComponentSerializerCreateFunc> _sCreateFunctions;
		return _sCreateFunctions;
	}
}