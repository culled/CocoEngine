#pragma once
#include "../../Physics3DPlatformFactory.h"

namespace Coco::Physics3D::Bullet
{
    class BulletPhysics3DPlatformFactory :
        public Physics3DPlatformFactory
    {
    public:
        // Inherited via Physics3DPlatformFactory
        UniqueRef<Physics3DPlatform> Create() const override;
    };
}
