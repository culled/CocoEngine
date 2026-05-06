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
    class BaseEntityComponentRegistry
    {
        template<typename FirstComponent, typename ... AdditionalComponents>
        friend class EntityComponentView;

    public:
        virtual ~BaseEntityComponentRegistry() = default;

        virtual bool Has(const UUID& entityID) const = 0;
        virtual EntityComponent* Get(const UUID& entityID) = 0;
        virtual const EntityComponent* Get(const UUID& entityID) const = 0;
        virtual void Remove(const UUID& entityID) = 0;
    };

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