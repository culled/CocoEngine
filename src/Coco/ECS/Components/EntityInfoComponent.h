#pragma once

#include "EntityComponent.h"
#include <Coco/Core/Types/String.h>
#include "../EntityTypes.h"

namespace Coco::ECS
{
    /// @brief A component that stores vital information for an entity
    class EntityInfoComponent :
        public EntityComponent
    {
        friend class EntityInfoComponentSerializer;
        friend class SceneSerializer;

    private:
        string _name;
        EntityID _entityID;
        bool _isActive;
        bool _isActiveInHierarchy;

    public:
        EntityInfoComponent(const Entity& owner);
        EntityInfoComponent(const Entity& owner, const string& name, const EntityID& entityID);

        /// @brief Sets the name of this entity
        /// @param name The entity's name
        void SetName(const string& name);

        /// @brief Gets the name of this entity
        /// @return The entity's name
        const string& GetName() const { return _name; }

        /// @brief Gets the ID of the entity
        /// @return The entity's ID
        const EntityID& GetEntityID() const { return _entityID; }

        /// @brief Sets the active state of this entity
        /// @param active The active state
        void SetActive(bool active);

        /// @brief Gets the active state of this entity. NOTE: if you want to test if this entity is active in the scene, use IsActiveInHierarchy()
        /// @return The active state of this entity
        bool GetIsActive() const { return _isActive; }

        /// @brief Determines if this entity is active in the scene
        /// @return True if this entity is active in the scene
        bool IsActiveInHierarchy() const { return _isActiveInHierarchy; }

    private:
        /// @brief Updates the scene visibility of this entity and its children
        /// @param isVisible The scene-visibility state
        void UpdateSceneVisibility(bool isVisible);
    };
}