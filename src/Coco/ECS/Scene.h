#pragma once

#include <Coco/Core/Resources/Resource.h>
#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/String.h>
#include <Coco/Core/MainLoop/TickListener.h>
#include <Coco/Core/Events/Event.h>
#include <Coco/Core/Types/TickSystem.h>

#include "Entity.h"
#include "SceneTickListener.h"

namespace Coco::ECS
{
    class Entity;
    class SceneSystem;

    /// @brief Simulate modes for a scene
    enum class SceneSimulateMode
    {
        Stopped,
        Running
    };

    /// @brief A collection of Entities
    class Scene :
        public Resource,
        public TickSystem<SceneTickListener, const TickInfo&>
    {
        friend class SceneSerializer;

    public:
        static const int TickPriority;

    public:
        Scene(const ResourceID& id);
        ~Scene();

        // Inherited from Resource
        const char* GetTypename() const override { return "Scene"; }
        const std::type_info& GetType() const override { return typeid(Scene); }

        /// @brief Creates an entity within this scene
        /// @param name The name of the entity
        /// @return The created entity
        Entity CreateEntity(const string& name = "Entity", const Entity& parent = Entity::Null);

        std::vector<Entity> GetEntities(bool onlyRootEntities);

        /// @brief Queues an entity for destruction at the end of the frame
        /// @param entity The entity to destroy
        void QueueDestroyEntity(Entity& entity);

        /// @brief Immediately destroys an entity
        /// @param entity The entity to destroy
        void DestroyEntity(Entity& entity);

        /// @brief Sets the simulate mode for this scene
        /// @param mode The simulate mode
        void SetSimulateMode(SceneSimulateMode mode);

        /// @brief Gets the simulate mode for this scene 
        /// @return This scene's simulate mode
        SceneSimulateMode GetSimulateMode() const { return _simulateMode; }

    private:
        ManagedRef<TickListener> _tickListener;
        SceneSimulateMode _simulateMode;
        std::vector<UniqueRef<SceneSystem>> _systems;

    private:
        static bool CompareTickListeners(const Ref<SceneTickListener>& a, const Ref<SceneTickListener>& b);
        static void PerformTick(Ref<SceneTickListener>& listener, const TickInfo& tickInfo);
        static bool HandleTickError(const std::exception& ex);

        /// @brief Handler for the tick
        /// @param tickInfo The tick info
        void HandleTick(const TickInfo& tickInfo);

        void DestroyAllEntities(bool destroyImmediately);
        void EntitiesAdded(std::span<Entity> rootEntities);

        void StartSimulation();
        void StopSimulation();
    };
}