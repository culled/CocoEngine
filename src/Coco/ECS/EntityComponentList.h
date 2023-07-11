#pragma once

#include <Coco/Core/Core.h>
#include "EntityTypes.h"
#include "EntityComponent.h"

#include <type_traits>

namespace Coco::ECS
{
	class IEntityComponentList
	{
	public:
		virtual ~IEntityComponentList() = default;

		virtual EntityComponent& Get(EntityID entityID) = 0;
		virtual const EntityComponent& Get(EntityID entityID) const = 0;
		virtual bool Has(EntityID entityID) const = 0;
		virtual bool Remove(EntityID entityID) = 0;
	};

	template <typename ComponentType>
	class EntityComponentList : public IEntityComponentList
	{
	private:
		UnorderedMap<uint64_t, ComponentType> _components;

	public:
		template<typename ... Args>
		ComponentType& Add(EntityID entity, Args&& ... args)
		{
			auto result = _components.try_emplace(entity, entity, std::forward<Args>(args)...).first;
			return result->second;
		}

		EntityComponent& Get(EntityID entity) final
		{
			return _components.at(entity);
		}

		const EntityComponent& Get(EntityID entity) const final
		{
			return _components.at(entity);
		}

		bool Has(EntityID entity) const final
		{
			return _components.contains(entity);
		}

		bool Remove(EntityID entity) final
		{
			return _components.erase(entity);
		}
	};
}