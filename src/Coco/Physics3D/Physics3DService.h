#pragma once

#include <Coco/Core/Services/EngineService.h>
#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/Singleton.h>
#include "Physics3DPlatform.h"
#include "Physics3DPlatformFactory.h"

namespace Coco::Physics3D
{
    class Physics3DService :
        public EngineService,
        public Singleton<Physics3DService>
    {
    private:
        UniqueRef<Physics3DPlatform> _platform;

    public:
        Physics3DService(const Physics3DPlatformFactory& platformFactory);
        ~Physics3DService();

        Physics3DPlatform& GetPlatform() { return *_platform; }
    };
}