#pragma once
#include <Coco/ECS/Components/EntityComponent.h>

namespace Coco
{
    class ViewportCameraControllerComponent :
        public ECS::EntityComponent
    {
    public:
        static const float MinMoveSpeed;
        static const float MaxMoveSpeed;

    public:
        ViewportCameraControllerComponent(const ECS::Entity& entity);

        // Inherited via EntityComponent
        const char* GetComponentTypename() const override { return "ViewportCameraControllerComponent"; }

        void Navigate();

    private:
        float _lookSensitivity;
        float _moveSpeed;
        float _panSpeed;
        std::array<bool, 2> _invertPan;
    };
}