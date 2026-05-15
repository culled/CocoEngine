//
// Created by cullen on 4/1/26.
//

#ifndef COCOENGINE_ECSSERVICE_H
#define COCOENGINE_ECSSERVICE_H
#include "Entity.h"
#include "EntityComponentStorage.h"
#include "EntityStorage.h"
#include "Coco/Core/EngineService.h"
#include "Coco/Core/ProcessLoop/TickInfo.h"
#include "Coco/Core/ProcessLoop/TickListener.h"
#include "Coco/Core/RTTI/RTTI.h"

namespace Coco
{
    /// @brief An EngineService that provides an entity-component service
    class ECSService : public EngineService
    {
    public:
        using SceneTickCallbackFunc = std::function<void(Scene& scene, const TickInfo&)>;

        /// @brief The tick order for safely destroying entities
        static constexpr int DestroyEntitiesTickOrder = 8000;

        /// @brief The tick order for safely destroying entities
        static constexpr int RootSceneTickOrder = -50;

        ECSService(Engine* engine, bool registerDefaultSystems = true);
        ~ECSService();

        /// @brief Creates an entity
        /// @param name The name of the entity
        /// @param owningScene The scene that will own the entity
        /// @param parentID The ID for the entity that will parent this new entity, or set to UUID::Nil to have this entity not be parented
        /// @return The entity
        Entity CreateEntity(const char* name, Scene& owningScene, const UUID& parentID);

        /// @brief Determines if the entity with the given ID exists
        /// @param entityID The ID of the entity
        /// @return True if the entity is valid
        bool IsEntityValid(const UUID& entityID) const;

        /// @brief Gets an entity by its ID
        /// @param entityID The ID of the entity
        /// @return The entity
        Entity GetEntity(const UUID& entityID);

        /// @brief Sets the name of an entity
        /// @param entityID The ID of the entity
        /// @param name The new name of the entity
        void SetEntityName(const UUID& entityID, const char* name);

        /// @brief Gets the name of an entity
        /// @param entityID The ID of the entity
        /// @return The name of the entity
        String GetEntityName(const UUID& entityID) const;

        /// @brief Sets the active state of an entity. Inactive entities also cause their descendents to become inactive
        /// @param entityID The ID of the entity
        /// @param isActive The active state of the entity
        void SetEntityIsActive(const UUID& entityID, bool isActive);

        /// @brief Gets the active state of an entity. NOTE: this does not take into account an entity's ancestor active states. Use IsEntityActiveInScene() to determine if an entity is active in the scene
        /// @param entityID The ID of the entity
        /// @return The entity's active state
        bool IsEntityActive(const UUID& entityID) const;

        /// @brief Gets the active state of an entity, taking into account the active states of all its ancestors
        /// @param entityID The ID of the entity
        /// @return The entity's active state, accounting for the active state of all its ancestors
        bool IsEntityActiveInScene(const UUID& entityID) const;

        /// @brief Gets the scene that owns an entity
        /// @param entityID The ID of the entity
        /// @return The entity's scene
        Scene* GetEntityScene(const UUID& entityID);

        /// @brief Gets the scene that owns an entity
        /// @param entityID The ID of the entity
        /// @return The entity's scene
        Scene* GetEntityScene(const UUID& entityID) const;

        /// @brief Determines if an entity has a parent
        /// @param entityID The ID of the entity
        /// @return True if the entity has a parent
        bool EntityHasParent(const UUID& entityID) const;

        /// @brief Gets the parent of an entity
        /// @param entityID The ID of the entity
        /// @return The entity's parent
        Entity GetEntityParent(const UUID& entityID);

        /// @brief Gets the number of children an entity has
        /// @param entityID The ID of the entity
        /// @return The number of children the entity has
        uint64 GetEntityChildCount(const UUID& entityID) const;

        /// @brief Immediately destroys an entity and all its descendents
        /// @param entityID The ID of the entity
        void DestroyEntityImmediate(const UUID& entityID);

        /// @brief Queues the entity and all its descendents for destruction
        /// @param entityID The ID of the entity
        void DestroyEntity(const UUID& entityID);

        /// @brief Gets the storage for entities
        /// @return The storage for entities
        EntityStorage& GetEntityStorage() { return _entities; }

        /// @brief Gets the storage for entities
        /// @return The storage for entities
        const EntityStorage& GetEntityStorage() const { return _entities; }

        /// @brief Gets the storage for entity components
        /// @return The storage for entity components
        EntityComponentStorage& GetComponentStorage() { return _components; }

        /// @brief Gets the storage for entity components
        /// @return The storage for entity components
        const EntityComponentStorage& GetComponentStorage() const { return _components; }

        /// @brief Registers a scene tick function callback
        /// @param sceneTickCallback The function that will be called during the scene tick
        /// @param order The tick order for the function. Callbacks are called in ascending order
        void RegisterSceneTickCallback(const SceneTickCallbackFunc& sceneTickCallback, int order);

        /// @brief Ticks the given scene. This is automatically called for root scenes when automatically ticking is enabled
        /// @param scene The scene being ticked
        /// @param tickInfo The tick info
        void TickScene(Scene& scene, const TickInfo& tickInfo);

        /// @brief Adds the given scene as a root scene. Root scenes are ticked and have other callbacks registered
        /// @param scene The scene to add
        void AddRootScene(SharedPtr<Scene> scene);

        /// @brief Removes the given scene from the list of root scenes
        /// @param scene The scene to remove
        void RemoveRootScene(const Scene& scene);

        /// @brief Enables or disables automatic ticking of all root scenes
        /// @param enabled If true, all root scenes will automatically be ticked
        void EnableTickingRootScenes(bool enabled);

    private:
        EntityComponentStorage _components;
        EntityStorage _entities;
        Array<UUID> _destroyEntityQueue;
        TickListener _destroyEntitiesTickListener;
        Array<std::pair<int, SceneTickCallbackFunc>> _sceneTickCallbacks;
        bool _sceneTickCallbacksNeedSorting;
        Array<SharedPtr<Scene>> _rootScenes;
        TickListener _rootSceneTickListener;

        /// @brief A tick handler for safely destroying entities
        /// @param tickInfo The tick info
        void DestroyEntityTick(const TickInfo& tickInfo);

        /// @brief A tick handler for ticking root scenes
        /// @param tickInfo The tick info
        void RootScenesTick(const TickInfo& tickInfo);

        /// @brief Sorts the scene tick callbacks
        void SortSceneTickCallbacks();

        /// @brief Registers all default systems
        void RegisterDefaultSystems();
    };
} // Coco

#endif //COCOENGINE_ECSSERVICE_H