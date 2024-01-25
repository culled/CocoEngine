#pragma once

#include "../../ECSpch.h"
#include "../../SceneSystem/SceneSystem.h"
#include "Rigidbody3DComponent.h"
#include "../Transform3DComponent.h"
#include <Coco/Physics3D/PhysicsWorld3D.h>
#include <Coco/Physics3D/CollisionShape3D.h>

namespace Coco::ECS
{
    class Physics3DSystem :
        public SceneSystem
    {
    public:
        static const int sSetupPriority;
        static const int sEarlyTickPriority;
        static const int sLateTickPriority;

    private:
        Ref<Physics3D::PhysicsWorld3D> _physicsWorld3D;

    public:
        Physics3DSystem(SharedRef<Scene> scene);
        ~Physics3DSystem();

        // Inherited via SceneSystem
        int GetSetupPriority() const override { return sSetupPriority; }
        void SimulationStarted() override;
        void SimulationEnded() override;

        static std::vector<SharedRef<Physics3D::CollisionShape3D>> GatherCollisionShapes(const Entity& entity);

        void HandleEarlyTick(const TickInfo& tickInfo);
        void HandleLateTick(const TickInfo& tickInfo);

    private:
        void CreateRigidbody(Rigidbody3DComponent& body, const Transform3DComponent& transform);
    };
}