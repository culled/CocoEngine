//
// Created by cullen on 4/2/26.
//

#ifndef COCOENGINE_ENTITYCOMPONENTREGISTRY_H
#define COCOENGINE_ENTITYCOMPONENTREGISTRY_H
#include "EntityComponent.h"
#include "Coco/Core/Types/Map.h"
#include "Coco/Core/Types/UUID.h"

namespace Coco
{
    /// @brief Base class for a registry of a single component type
    class BaseEntityComponentRegistry
    {
        template<typename FirstComponent, typename ... AdditionalComponents>
        friend class EntityComponentView;

    public:
        virtual ~BaseEntityComponentRegistry() = default;

        /// @brief Determines if a component in this registry is attached to the given entity
        /// @param entityID The ID of the entity
        /// @return True if a component exists for the entity
        virtual bool Has(const UUID& entityID) const = 0;

        /// @brief Gets a component in this registry that is attached to the given entity
        /// @param entityID The ID of the entity
        /// @return The component attached to the entity
        virtual EntityComponent* Get(const UUID& entityID) = 0;

        /// @brief Gets a component in this registry that is attached to the given entity
        /// @param entityID The ID of the entity
        /// @return The component attached to the entity
        virtual const EntityComponent* Get(const UUID& entityID) const = 0;

        /// @brief Removes a component from this registry that is attached to the given entity
        /// @param entityID The ID of the entity
        virtual void Remove(const UUID& entityID) = 0;
    };

    /// @brief A registry for a component that can be attached to entities
    /// @tparam ComponentType The component type
    template<typename ComponentType>
    class EntityComponentRegistry : public BaseEntityComponentRegistry
    {
        template<typename FirstComponent, typename ... AdditionalComponents>
        friend class EntityComponentView;

    public:
        bool Has(const UUID& entityID) const override { return _components.Contains(entityID); }
        EntityComponent* Get(const UUID& entityID) override { return _components.TryGetValue(entityID); }
        const EntityComponent* Get(const UUID& entityID) const override { return _components.TryGetValue(entityID); }
        void Remove(const UUID& entityID) override { _components.Remove(entityID); }

        /// @brief Creates a component in this registry for a given entity
        /// @tparam Args The constructor arguments
        /// @param entityID The ID of the entity
        /// @param args The arguments to pass to the component's constructor
        /// @return The component
        template<typename ... Args>
        ComponentType* Create(const UUID& entityID, Args&& ... args)
        {
            return &_components.Emplace(entityID, entityID, std::forward<Args>(args)...);
        }

    private:
        Map<UUID, ComponentType> _components;
    };
}

#endif //COCOENGINE_ENTITYCOMPONENTREGISTRY_H