#pragma once

#include <Coco/Core/Core.h>
#include "EntityTypes.h"
#include "EntityComponent.h"

#include <type_traits>

namespace Coco::ECS
{
	/// @brief A generic list of EntityComponents
	class IEntityComponentList
	{
	public:
		virtual ~IEntityComponentList() = default;

		/// @brief Gets a component for an entity from this list
		/// @param entityID The ID of the entity
		/// @return The component
		virtual EntityComponent& Get(const EntityID& entityID) = 0;

		/// @brief Gets a component for an entity from this list
		/// @param entityID The ID of the entity
		/// @return The component
		virtual const EntityComponent& Get(const EntityID& entityID) const = 0;

		/// @brief Checks if the given entity has a component managed by this component list
		/// @param entityID The ID of the entity
		/// @return True if the entity has a component managed by this component list
		virtual bool Has(const EntityID& entityID) const = 0;

		/// @brief Removes a component in this list from the given entity
		/// @param entityID The entity ID
		/// @return True if a component managed by this list was found and removed from the given entity
		virtual bool Remove(const EntityID& entityID) = 0;
	};

	/// @brief Manages a list of a EntityComponents
	/// @tparam ComponentType The type of component
	template <typename ComponentType>
	class EntityComponentList : public IEntityComponentList
	{
	private:
		UnorderedMap<EntityID, ComponentType> _components;

	public:
		/// @brief Creates a component and attaches it to the given entity
		/// @tparam ...Args The types of constructor arguments
		/// @param entity The entity ID
		/// @param ...args Arguments to pass to the component's constructor
		/// @return The component
		template<typename ... Args>
		ComponentType& Add(const EntityID& entity, Args&& ... args)
		{
			auto result = _components.try_emplace(entity, entity, std::forward<Args>(args)...).first;
			return result->second;
		}

		/// @brief Gets a component for the given entity
		/// @param entity The entity ID
		/// @return The component attached to the given entity
		EntityComponent& Get(const EntityID& entity) final
		{
			return _components.at(entity);
		}

		/// @brief Gets a component for the given entity
		/// @param entity The entity ID
		/// @return The component attached to the given entity
		const EntityComponent& Get(const EntityID& entity) const final
		{
			return _components.at(entity);
		}

		/// @brief Checks if a component exists for the given entity
		/// @param entity The entity ID
		/// @return True if a component exists for the given entity
		bool Has(const EntityID& entity) const final
		{
			return _components.contains(entity);
		}

		/// @brief Removes a component from the given entity
		/// @param entity The entity ID
		/// @return True if a component was found and removed from the given entity
		bool Remove(const EntityID& entity) final
		{
			return _components.erase(entity);
		}
	};
}