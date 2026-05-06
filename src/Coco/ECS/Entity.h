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

    class Entity
    {
        friend class EntityChildView;

    public:
        Entity();
        Entity(ECSService* ecs, const UUID& id);

        operator UUID() const { return _id; }
        operator bool() const { return IsValid(); }

        const UUID& GetID() const { return _id; }
        bool IsValid() const;
        void SetName(const char* name);
        String GetName() const;

        void SetIsActive(bool isActive);
        bool IsSelfActive() const;
        bool IsActiveInScene() const;

        Scene* GetScene();
        Scene* GetScene() const;

        bool HasParent() const;
        Entity GetParent() const;

        uint64 GetChildCount() const;

        EntityChildView GetChildren();

        template<typename ComponentType, typename... Args>
        ComponentType* CreateComponent(Args&& ... args)
        {
            if (!_componentStorage)
                return nullptr;

            return _componentStorage->CreateEntityComponent<ComponentType>(_id, std::forward<Args>(args)...);
        }

        template<typename ComponentType>
        bool HasComponent() const
        {
            return HasComponent(ComponentType::GetClassRTTI());
        }

        bool HasComponent(const ClassRTTI& componentType) const;

        template<typename ComponentType>
        ComponentType* GetComponent()
        {
            return static_cast<ComponentType*>(GetComponent(ComponentType::GetClassRTTI()));
        }

        template<typename ComponentType>
        const ComponentType* GetComponent() const
        {
            return static_cast<const ComponentType*>(GetComponent(ComponentType::GetClassRTTI()));
        }

        EntityComponent* GetComponent(const ClassRTTI& componentType);
        const EntityComponent* GetComponent(const ClassRTTI& componentType) const;

        template<typename ComponentType>
        void RemoveComponent()
        {
            RemoveComponent(ComponentType::GetClassRTTI());
        }

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