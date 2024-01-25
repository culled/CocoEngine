#pragma once

#include "../Corepch.h"
#include "../Types/Refs.h"
#include "../Types/String.h"

namespace Coco
{
	class ResourceSerializer;

	class ResourceSerializerFactory
	{
	public:
		using ResourceSerializerCreateFunc = std::function<UniqueRef<ResourceSerializer>()>;

	public:
		static UniqueRef<ResourceSerializer> Create(const string& resourceTypename);

		template<class SerializerType>
		static void Register(const string& resourceTypename)
		{
			GetMap().try_emplace(resourceTypename, Instatiate<SerializerType>);
		}

	private:
		static std::unordered_map<string, ResourceSerializerCreateFunc>& GetMap();

	private:
		ResourceSerializerFactory() = delete;

		template<class SerializerType>
		static UniqueRef<ResourceSerializer> Instatiate()
		{
			return CreateUniqueRef<SerializerType>();
		}
	};
}