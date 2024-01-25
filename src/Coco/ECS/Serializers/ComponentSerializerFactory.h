#pragma once

#include "../ECSpch.h"
#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/String.h>
#include "../Entity.h"

namespace Coco::ECS
{
	class BaseComponentSerializer;

	class ComponentSerializerFactory
	{
		friend class SceneSerializer;

	public:
		using ComponentSerializerCreateFunc = std::function<UniqueRef<BaseComponentSerializer>()>;

	public:
		static UniqueRef<BaseComponentSerializer> Create(const string& componentTypename);
		static bool Has(const string& componentTypename);

		template<class SerializerType>
		static void Register(const string& componentTypename)
		{
			GetMap().try_emplace(componentTypename, Instatiate<SerializerType>);
		}

	private:
		static std::unordered_map<string, ComponentSerializerCreateFunc>& GetMap();

	private:
		ComponentSerializerFactory() = delete;

		template<class SerializerType>
		static UniqueRef<BaseComponentSerializer> Instatiate()
		{
			return CreateUniqueRef<SerializerType>();
		}
	};
}

