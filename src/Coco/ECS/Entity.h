//
// Created by cullen on 4/1/26.
//

#ifndef COCOENGINE_ENTITY_H
#define COCOENGINE_ENTITY_H
#include "EntityComponent.h"
#include "EntityComponentStorage.h"
#include "Coco/Core/Types/UUID.h"

namespace Coco
{
    class Scene;
    class ECSService;
    class EntityChildView;

    /// @brief An entity that can have components attached to it
    class Entity
    {
        friend class EntityChildView;

    public:
        Entity();
        Entity(ECSService* ecs, const UUID& id);

        operator UUID() const { return _id; }
        operator bool() const { return IsValid(); }

        /// @brief Gets the ID of this entity
        /// @return The ID of this entity
        const UUID& GetID() const { return _id; }

        /// @brief Determines if this entity is valid
        /// @return True if this entity exists and is valid
        bool IsValid() const;

        /// @brief Sets the name of this entity
        /// @param name The new name
        void SetName(const char* name);

        /// @brief Gets the name of this entity
        /// @return This entity's name
        String GetName() const;

        /// @brief Sets the active state of this entity
        /// @param isActive The new active state
        void SetIsActive(bool isActive);

        /// @brief Gets the active state of this entity. NOTE: this is unaffected by the active states of this entity's ancestors
        /// @return This entity's active state
        bool IsSelfActive() const;

        /// @brief Gets the active state of this entity, taking into account the active states of all its ancestors
        /// @return True if this entity and all its ancestors are active
        bool IsActiveInScene() const;

        /// @brief Gets the scene that owns this entity
        /// @return The scene that owns this entity
        Scene* GetScene();

        /// @brief Gets the scene that owns this entity
        /// @return The scene that owns this entity
        Scene* GetScene() const;

        /// @brief Determines if this entity has a parent
        /// @return True if this entity has a parent
        bool HasParent() const;

        /// @brief Gets this entity's parent
        /// @return This entity's parent, or an invalid entity if this entity has no parent
        Entity GetParent() const;

        /// @brief Gets the number of children this entity has
        /// @return The number of children this entity has
        uint64 GetChildCount() const;

        /// @brief Gets an iterable view over this entity's children
        /// @return An iterable view over this entity's children
        EntityChildView GetChildren();

        /// @brief Creates a component for this entity
        /// @tparam ComponentType The type of component
        /// @tparam Args The component's constructor arguments
        /// @param args Arguments to pass to the component's constructor
        /// @return The component
        template<typename ComponentType, typename... Args>
        ComponentType* CreateComponent(Args&& ... args)
        {
            if (!_componentStorage)
                return nullptr;

            return _componentStorage->CreateEntityComponent<ComponentType>(_id, std::forward<Args>(args)...);
        }

        /// @brief Determines if this entity has a component of the given type
        /// @tparam ComponentType The type of component
        /// @return True if this entity has a component of the given type
        template<typename ComponentType>
        bool HasComponent() const
        {
            return HasComponent(ComponentType::GetClassRTTI());
        }

        /// @brief Determines if this entity has a component of the given type
        /// @param componentType The component type information
        /// @return True if this entity has a component of the given type
        bool HasComponent(const ClassRTTI& componentType) const;

        /// @brief Gets a component of this entity
        /// @tparam ComponentType The type of component
        /// @return The component, or nullptr if this entity doesn't have the given component
        template<typename ComponentType>
        ComponentType* GetComponent()
        {
            return static_cast<ComponentType*>(GetComponent(ComponentType::GetClassRTTI()));
        }

        /// @brief Gets a component of this entity
        /// @tparam ComponentType The type of component
        /// @return The component, or nullptr if this entity doesn't have the given component
        template<typename ComponentType>
        const ComponentType* GetComponent() const
        {
            return static_cast<const ComponentType*>(GetComponent(ComponentType::GetClassRTTI()));
        }

        /// @brief Gets a component of this entity
        /// @param componentType The component type information
        /// @return The component, or nullptr if this entity doesn't have the given component
        EntityComponent* GetComponent(const ClassRTTI& componentType);

        /// @brief Gets a component of this entity
        /// @param componentType The component type information
        /// @return The component, or nullptr if this entity doesn't have the given component
        const EntityComponent* GetComponent(const ClassRTTI& componentType) const;

        /// @brief Removes a component from this entity
        /// @tparam ComponentType The type of component
        template<typename ComponentType>
        void RemoveComponent()
        {
            RemoveComponent(ComponentType::GetClassRTTI());
        }

        /// @brief Removes a component from this entity
        /// @param componentType The component type information
        void RemoveComponent(const ClassRTTI& componentType);

    private:
        ECSService* _ecs;
        UUID _id;
        EntityComponentStorage* _componentStorage;
    };

    bool operator==(const Entity& entity, const Entity& other);
    inline bool operator!=(const Entity& entity, const Entity& other) { return !(entity == other); }
} // Coco

#endif //COCOENGINE_ENTITY_H