#pragma once

#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/String.h>
#include "entt.h"

namespace Coco::ECS
{
	/// @brief An object that components can be attached to
	class Entity
	{
		friend class ECSService;

		template<typename, typename...>
		friend class ComponentView;

	public:
		static const Entity Null;

	public:
		Entity();
		Entity(const Entity& other);

		bool operator==(const Entity& other) const;
		bool operator!=(const Entity& other) const { return !(*this == other); }

		operator uint64() const { return static_cast<uint64>(_handle); }

		/// @brief Checks if this entity is valid
		/// @return True if this entity is valid and can be used
		bool IsValid() const;

		/// @brief Adds a component to this entity
		/// @tparam ComponentType The type of component
		/// @tparam ...Args The component constructor argument types
		/// @param ...args The arguments to pass to the component's constructor
		/// @return The component
		template<typename ComponentType, typename ... Args>
		ComponentType& AddComponent(Args&& ... args);

		/// @brief Determines if this entity has a component of the given type
		/// @tparam ComponentType The type of component
		/// @return True if this entity has a component of the given type
		template<typename ComponentType>
		bool HasComponent() const;

		template<typename ComponentType>
		ComponentType& GetComponent();

		/// @brief Gets a component that is attached to this entity
		/// @tparam ComponentType The type of component
		/// @return The component
		template<typename ComponentType>
		const ComponentType& GetComponent() const;

		/// @brief Removes a component from this entity
		/// @tparam ComponentType The type of component to remove
		template<typename ComponentType>
		void RemoveComponent();

		/// @brief Tries to find a component of the given type on this entity and returns it if found
		/// @tparam ComponentType The type of component to get
		/// @param component Will be set to the component if found
		/// @return True if the component was found
		template<typename ComponentType>
		bool TryGetComponent(const ComponentType*& outComponent) const
		{
			if (!HasComponent<ComponentType>())
				return false;

			outComponent = &GetComponent<ComponentType>();
			return true;
		}

		template<typename ComponentType>
		bool TryGetComponent(ComponentType*& outComponent)
		{
			if (!HasComponent<ComponentType>())
				return false;

			outComponent = &GetComponent<ComponentType>();
			return true;
		}

		void SetParent(const Entity& parent);
		void ClearParent();

		std::vector<Entity> GetChildren() const;
		Entity GetParent() const;
		bool IsOrphaned() const;
		bool IsDescendentOf(const Entity& ancestor) const;

		bool IsActiveInHierarchy() const;

		const string& GetName() const;

	private:
		entt::entity _handle;

	private:
		Entity(entt::entity handle);
	};
}

#include "ECSService.h"

namespace Coco::ECS
{
	template<typename ComponentType, typename ... Args>
	ComponentType& Entity::AddComponent(Args&& ... args)
	{
		return ECSService::Get()->CreateComponentOnEntity<ComponentType>(*this, std::forward<Args>(args)...);
	}

	template<typename ComponentType>
	bool Entity::HasComponent() const
	{
		return ECSService::Get()->EntityHasComponent<ComponentType>(*this);
	}

	template<typename ComponentType>
	ComponentType& Entity::GetComponent()
	{
		return ECSService::Get()->GetEntityComponent<ComponentType>(*this);
	}

	template<typename ComponentType>
	const ComponentType& Entity::GetComponent() const
	{
		return ECSService::Get()->GetEntityComponent<ComponentType>(*this);
	}

	template<typename ComponentType>
	void Entity::RemoveComponent()
	{
		return ECSService::Get()->RemoveEntityComponent<ComponentType>(*this);
	}
}