//
// Created by cullen on 4/2/26.
//

#ifndef COCOENGINE_ENTITYCOMPONENTSTORAGE_H
#define COCOENGINE_ENTITYCOMPONENTSTORAGE_H
#include "EntityComponentRegistry.h"
#include "Coco/Core/Memory/Ptrs.h"
#include "EntityComponent.h"

namespace Coco
{
    /// @brief Manages components for entities
    class EntityComponentStorage
    {
        template<typename FirstComponent, typename ... AdditionalComponents>
        friend class EntityComponentView;

    public:
        /// @brief Creates a component for an entity
        /// @tparam ComponentType The type of component
        /// @tparam Args The constructor argument types
        /// @param entityID The ID of the entity
        /// @param args The arguments to pass to the component's constructor
        /// @return The component
        template<typename ComponentType, typename ... Args>
        ComponentType* CreateEntityComponent(const UUID& entityID, Args&& ... args)
        {
            auto registry = GetOrCreateRegistry<ComponentType>();
            return registry->Create(entityID, std::forward<Args>(args)...);
        }

        /// @brief Determines if an entity has a component matching the given type
        /// @param entityID The ID of the entity
        /// @param componentType The type information of the component
        /// @return True if the entity has a component of the given type
        bool Exists(const UUID& entityID, const ClassRTTI& componentType) const;

        /// @brief Gets a component for an entity that matches the given type
        /// @param entityID The ID of the entity
        /// @param componentType The type information of the component
        /// @return The component
        EntityComponent* Get(const UUID& entityID, const ClassRTTI& componentType);

        /// @brief Gets a component for an entity that matches the given type
        /// @param entityID The ID of the entity
        /// @param componentType The type information of the component
        /// @return The component
        const EntityComponent* Get(const UUID& entityID, const ClassRTTI& componentType) const;

        /// @brief Removes a component matching the given type from an entity
        /// @param entityID The ID of the entity
        /// @param componentType The type information of the component
        void Remove(const UUID& entityID, const ClassRTTI& componentType);

        /// @brief Removes all components from the given entity
        /// @param entityID The ID of the entity
        void RemoveAll(const UUID& entityID);

        /// @brief Clears all components
        void Clear();

    private:
        Map<ClassRTTI, UniquePtr<BaseEntityComponentRegistry>> _registries;

        /// @brief Gets or creates a registry for a type of component
        /// @tparam ComponentType The type of component
        /// @return The registry for the type of component
        template<typename ComponentType>
        EntityComponentRegistry<ComponentType>* GetOrCreateRegistry()
        {
            const ClassRTTI& type = ComponentType::GetClassRTTI();
            UniquePtr<BaseEntityComponentRegistry>* registry = _registries.TryGetValue(type);
            if (!registry)
                registry = &_registries.Emplace(type, CreateDefaultUnique<EntityComponentRegistry<ComponentType>>());

            return static_cast<EntityComponentRegistry<ComponentType>*>(registry->get());
        }
    };
} // Coco

#endif //COCOENGINE_ENTITYCOMPONENTSTORAGE_H