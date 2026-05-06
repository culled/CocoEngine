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
    class EntityComponentStorage
    {
        template<typename FirstComponent, typename ... AdditionalComponents>
        friend class EntityComponentView;

    public:
        template<typename ComponentType, typename ... Args>
        ComponentType* CreateEntityComponent(const UUID& entityID, Args&& ... args)
        {
            auto registry = GetOrCreateRegistry<ComponentType>();
            return registry->Create(entityID, std::forward<Args>(args)...);
        }

        bool Exists(const UUID& entityID, const ClassRTTI& componentType) const;
        EntityComponent* Get(const UUID& entityID, const ClassRTTI& componentType);
        const EntityComponent* Get(const UUID& entityID, const ClassRTTI& componentType) const;

        void Remove(const UUID& entityID, const ClassRTTI& componentType);
        void RemoveAll(const UUID& entityID);

        void Clear();

    private:
        Map<ClassRTTI, UniquePtr<BaseEntityComponentRegistry>> _registries;

    private:
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