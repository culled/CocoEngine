#include "ECSpch.h"
#include "ECSService.h"
#include "Components/EntityInfoComponent.h"

#include "Serializers/ComponentSerializerFactory.h"
#include "Serializers/Components/EntityInfoComponentSerializer.h"
#include "Serializers/Components/Transform3DComponentSerializer.h"

#ifdef COCO_SERVICE_RENDERING
#include "Serializers/Components/Rendering/CameraComponentSerializer.h"
#include "Serializers/Components/Rendering/MeshRendererComponentSerializer.h"
#endif

#include "Systems/SceneSystemFactory.h"
#include "Systems/TransformSystem.h"

#include <Coco/Core/Resources/ResourceSerializerFactory.h>
#include "Serializers/SceneSerializer.h"

namespace Coco::ECS
{
    const int ECSService::EarlyTickPriority = -10000;

    ECSService::ECSService() :
        _registry(),
        _earlyTickListener(CreateManagedRef<TickListener>(this, &ECSService::HandleEarlyTick, EarlyTickPriority)),
        _queuedDestroyEntities(),
        _entityParentMap()
    {
        MainLoop::Get()->AddTickListener(_earlyTickListener);

        RegisterComponentSerializers();
        RegisterSceneSystems();
        RegisterResourceSerializers();
    }

    ECSService::~ECSService()
    {
        MainLoop::Get()->RemoveTickListener(_earlyTickListener);
    }

    Entity ECSService::CreateEntity(const string& name, const Entity& parent, const SharedRef<Scene>& scene)
    {
        Entity e(_registry.create());
        CreateComponentOnEntity<EntityInfoComponent>(e, name, scene);

        if (parent != Entity::Null)
            SetEntityParent(e, parent);

        return e;
    }

    bool ECSService::IsEntityValid(const Entity& entity) const
    {
        return _registry.valid(entity._handle);
    }

    void ECSService::QueueDestroyEntity(Entity& entity)
    {
        if (!IsEntityValid(entity))
            return;

        auto it = std::find(_queuedDestroyEntities.begin(), _queuedDestroyEntities.end(), entity);

        if (it != _queuedDestroyEntities.end())
            return;

        _queuedDestroyEntities.push_back(entity);
    }

    void ECSService::DestroyEntity(Entity& entity)
    {
        if (!IsEntityValid(entity))
            return;

        // Destroy this entity's children
        std::vector<Entity> children = GetEntityChildren(entity);
        for (auto& child : children)
            DestroyEntity(child);

        OnEntityDestroyed.Invoke(entity);
        _entityParentMap.erase(entity._handle);

        _registry.destroy(entity._handle);
        entity._handle = entt::null;
    }

    void ECSService::SetEntityParent(Entity& entity, const Entity& parent)
    {
        if (entity == parent)
            return;

        if (parent == Entity::Null)
            _entityParentMap.erase(entity._handle);
        else
            _entityParentMap[entity._handle] = parent._handle;
    }

    void ECSService::ClearEntityParent(Entity& entity)
    {
        SetEntityParent(entity, Entity::Null);
    }

    Entity ECSService::GetEntityParent(const Entity& entity) const
    {
        auto it = _entityParentMap.find(entity._handle);
        if (it == _entityParentMap.end())
            return Entity::Null;

        return Entity(it->second);
    }

    bool ECSService::IsEntityOrphaned(const Entity& entity) const
    {
        return !_entityParentMap.contains(entity._handle);
    }

    std::vector<Entity> ECSService::GetEntityChildren(const Entity& entity) const
    {
        std::vector<Entity> children;
        for (auto& childKvp : _entityParentMap)
        {
            if (childKvp.second != entity._handle)
                continue;

            children.push_back(Entity(childKvp.first));
        }

        return children;
    }

    bool ECSService::IsEntityDescendentOf(const Entity& entity, const Entity& ancestor) const
    {
        if (ancestor == entity)
            return true;

        Entity parent = GetEntityParent(entity);
        if (parent == Entity::Null)
            return false;

        return IsEntityDescendentOf(parent, ancestor);
    }

    std::vector<Entity> ECSService::GetSceneEntities(const Scene& scene, bool rootEntitiesOnly) const
    {
        std::vector<Entity> sceneEntities;

        auto view = _registry.view<EntityInfoComponent>();
        for (const auto& e : view)
        {
            const auto& entityInfo = _registry.get<EntityInfoComponent>(e);
            SharedRef<Scene> entityScene = entityInfo.GetScene();

            if (!entityScene || entityScene.get() != &scene)
                continue;

            if (!rootEntitiesOnly || IsEntityOrphaned(e))
            {
                sceneEntities.push_back(Entity(e));
            }
        }

        return sceneEntities;
    }

    void ECSService::RegisterComponentSerializers()
    {
        ComponentSerializerFactory::Register<EntityInfoComponentSerializer>("EntityInfoComponent");
        ComponentSerializerFactory::Register<Transform3DComponentSerializer>("Transform3DComponent");

#ifdef COCO_SERVICE_RENDERING
        ComponentSerializerFactory::Register<CameraComponentSerializer>("CameraComponent");
        ComponentSerializerFactory::Register<MeshRendererComponentSerializer>("MeshRendererComponent");
#endif
    }

    void ECSService::RegisterSceneSystems()
    {
        SceneSystemFactory::Register<TransformSystem>();
    }

    void ECSService::RegisterResourceSerializers()
    {
        ResourceSerializerFactory::Register<SceneSerializer>("Scene");
    }

    void ECSService::HandleEarlyTick(const TickInfo& tickInfo)
    {
        for(auto& e : _queuedDestroyEntities)
            DestroyEntity(e);

        _queuedDestroyEntities.clear();
    }
}