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

		virtual EntityComponent& Get(const EntityID& entityID) = 0;
		virtual const EntityComponent& Get(const EntityID& entityID) const = 0;
		virtual bool Has(const EntityID& entityID) const = 0;
		virtual bool Remove(const EntityID& entityID) = 0;
	};

	template <typename ComponentType>
	class EntityComponentList : public IEntityComponentList
	{
	private:
		UnorderedMap<EntityID, ComponentType> _components;

	public:
		template<typename ... Args>
		ComponentType& Add(const EntityID& entity, Args&& ... args)
		{
			auto result = _components.try_emplace(entity, entity, std::forward<Args>(args)...).first;
			return result->second;
		}

		EntityComponent& Get(const EntityID& entity) final
		{
			return _components.at(entity);
		}

		const EntityComponent& Get(const EntityID& entity) const final
		{
			return _components.at(entity);
		}

		bool Has(const EntityID& entity) const final
		{
			return _components.contains(entity);
		}

		bool Remove(const EntityID& entity) final
		{
			return _components.erase(entity);
		}
	};
}