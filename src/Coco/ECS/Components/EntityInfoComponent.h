#pragma once

#include "EntityComponent.h"
#include <Coco/Core/Types/String.h>

namespace Coco::ECS
{
    class Scene;

    /// @brief A component that stores vital information for an entity
    class EntityInfoComponent :
        public EntityComponent
    {
        friend class Scene;
        friend class EntityInfoComponentSerializer;

    public:
        EntityInfoComponent(const Entity& owner);
        EntityInfoComponent(const Entity& owner, const string& name, const SharedRef<Scene>& scene);

        // Inherited via EntityComponent
        const char* GetComponentTypename() const override { return "EntityInfoComponent"; }

        /// @brief Sets the name of this entity
        /// @param name The entity's name
        void SetName(const string& name);

        /// @brief Gets the name of this entity
        /// @return The entity's name
        const string& GetName() const { return _name; }

        SharedRef<Scene> GetScene() const { return _scene.expired() ? nullptr : _scene.lock(); }

        void SetSelfActive(bool isActive);
        bool GetIsSelfActive() const { return _isSelfActive; }
        bool IsActiveInHierarchy() const { return _isActiveInHierarchy; }

    private:
        string _name;
        bool _isSelfActive;
        bool _isActiveInHierarchy;
        WeakSharedRef<Scene> _scene;

    private:
        void SetScene(SharedRef<Scene> scene);
        void UpdateSceneVisibility(bool isParentVisible);
    };
}