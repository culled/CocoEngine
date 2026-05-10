//
// Created by cullen on 4/1/26.
//

#include "Entity.h"
#include "EntityChildView.h"
#include "ECSService.h"

namespace Coco
{
    Entity::Entity() :
        _ecs(nullptr),
        _id(UUID::Nil),
        _componentStorage(nullptr)
    {}

    Entity::Entity(ECSService* ecs, const UUID& id) :
        _ecs(ecs),
        _id(id),
        _componentStorage(&ecs->GetComponentStorage())
    {}

    bool Entity::IsValid() const
    {
        if (!_ecs || _id == UUID::Nil)
            return false;

        return _ecs->IsEntityValid(_id);
    }

    void Entity::SetName(const char* name)
    {
        if (_ecs)
            _ecs->SetEntityName(_id, name);
    }

    String Entity::GetName() const
    {
        if (_ecs)
            return _ecs->GetEntityName(_id);

        return String();
    }

    void Entity::SetIsActive(bool isActive)
    {
        if (_ecs)
            _ecs->SetEntityIsActive(_id, isActive);
    }

    bool Entity::IsSelfActive() const
    {
        if (_ecs)
            return _ecs->IsEntityActive(_id);

        return false;
    }

    bool Entity::IsActiveInScene() const
    {
        if (_ecs)
            return _ecs->IsEntityActiveInScene(_id);

        return false;
    }

    Scene* Entity::GetScene()
    {
        return _ecs->GetEntityScene(_id);
    }

    Scene* Entity::GetScene() const
    {
        return _ecs->GetEntityScene(_id);
    }

    bool Entity::HasParent() const
    {
        if (_ecs)
            return _ecs->EntityHasParent(_id);

        return false;
    }

    Entity Entity::GetParent() const
    {
        if (_ecs)
            return _ecs->GetEntityParent(_id);

        return Entity();
    }

    uint64 Entity::GetChildCount() const
    {
        if (_ecs)
            return _ecs->GetEntityChildCount(_id);

        return 0;
    }

    EntityChildView Entity::GetChildren()
    {
        return EntityChildView(*this);
    }

    bool Entity::HasComponent(const ClassRTTI& componentType) const
    {
        if (!_componentStorage)
            return false;

        return _componentStorage->Exists(_id, componentType);
    }

    EntityComponent* Entity::GetComponent(const ClassRTTI& componentType)
    {
        if (!_componentStorage)
            return nullptr;

        return _componentStorage->Get(_id, componentType);
    }

    const EntityComponent* Entity::GetComponent(const ClassRTTI& componentType) const
    {
        if (!_componentStorage)
            return nullptr;

        return _componentStorage->Get(_id, componentType);
    }

    void Entity::RemoveComponent(const ClassRTTI& componentType)
    {
        if (!_componentStorage)
            return;

        _componentStorage->Remove(_id, componentType);
    }

    bool operator==(const Entity& entity, const Entity& other)
    {
        return entity.GetID() == other.GetID();
    }
} // Coco