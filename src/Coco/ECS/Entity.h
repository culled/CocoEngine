#pragma once

#include <Coco/Core/Types/Refs.h>
#include "Scene.h"
#include "entt.h"

namespace Coco::ECS
{
	/// @brief An object that components can be attached to
	class Entity
	{
		friend class Scene;

		template<typename, typename...>
		friend class SceneViewIterator;

	public:
		static const Entity Null;

	private:
		entt::entity _handle;
		WeakSharedRef<Scene> _scene;

	private:
		Entity(entt::entity handle, SharedRef<Scene> scene);

	public:
		Entity();
		Entity(const Entity& other);

		bool operator==(const Entity& other) const;
		bool operator!=(const Entity& other) const { return !(*this == other); }

		/// @brief Checks if this entity is still valid
		/// @return True if this entity is valid and can be used
		bool IsValid() const;

		/// @brief Gets the ID of this entity
		/// @return This entity's ID
		const EntityID& GetID() const;

		/// @brief Gets the scene that this entity belongs to
		/// @return The scene
		SharedRef<Scene> GetScene() const;

		/// @brief Determines if this entity is active in the scene hierarchy
		/// @return True if this entity is active
		bool IsActiveInHierarchy() const;

		/// @brief Adds a component to this entity
		/// @tparam ComponentType The type of component
		/// @tparam ...Args The component constructor argument types
		/// @param ...args The arguments to pass to the component's constructor
		/// @return The component
		template<typename ComponentType, typename ... Args>
		ComponentType& AddComponent(Args&& ... args)
		{
			Assert(IsValid())
			Assert(!HasComponent<ComponentType>())

			SharedRef<Scene> s = _scene.lock();
			return s->_registry.emplace<ComponentType>(_handle, *this, std::forward<Args>(args)...);
		}

		/// @brief Determines if this entity has a component of the given type
		/// @tparam ComponentType The type of component
		/// @return True if this entity has a component of the given type
		template<typename ComponentType>
		bool HasComponent() const
		{
			Assert(IsValid())

			SharedRef<Scene> s = _scene.lock();
			return s->_registry.all_of<ComponentType>(_handle);
		}

		/// @brief Gets a component that is attached to this entity
		/// @tparam ComponentType The type of component
		/// @return The component
		template<typename ComponentType>
		ComponentType& GetComponent() const
		{
			Assert(IsValid())
			Assert(HasComponent<ComponentType>())

			SharedRef<Scene> s = _scene.lock();
			return s->_registry.get<ComponentType>(_handle);
		}

		/// @brief Removes a component from this entity
		/// @tparam ComponentType The type of component to remove
		template<typename ComponentType>
		void RemoveComponent()
		{
			Assert(IsValid())
			Assert(HasComponent<ComponentType>())

			SharedRef<Scene> s = _scene.lock();
			s->_registry.remove<ComponentType>(_handle);
		}

		/// @brief Tries to find a component of the given type on this entity and returns it if found
		/// @tparam ComponentType The type of component to get
		/// @param component Will be set to the component if found
		/// @return True if the component was found
		template<typename ComponentType>
		bool TryGetComponent(ComponentType*& component) const
		{
			Assert(IsValid())

			SharedRef<Scene> s = _scene.lock();
			component = s->_registry.try_get<ComponentType>(_handle);

			return component != nullptr;
		}

		/// @brief Sets this entity's parent
		/// @param parent The parent
		void SetParent(const Entity& parent);

		/// @brief Determines if this entity has a parent
		/// @return True if this entity has a parent
		bool HasParent() const;

		/// @brief Clears this entity's parent
		void ClearParent();
		
		/// @brief Determines if the given entity is an ancestor to this entity (i.e. this entity is somehow a child of the ancestor)
		/// @param ancestor The ancestor entity
		/// @return True if this entity is a descendent of the ancestor entity
		bool IsDescendentOf(const Entity& ancestor) const;

		/// @brief Gets this entity's parent, if it has one
		/// @return The parent entity, or a null entity if this entity does not have a parent
		Entity GetParent() const;

		/// @brief Gets the children of this entity
		/// @return The child entities
		std::vector<Entity> GetChildren() const;
	};
}