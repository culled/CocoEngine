//
// Created by cullen on 2/24/26.
//

#include "EnginePlatform.h"

#include "Asserts.h"
#include "Engine.h"
#include "Memory/MemoryOverrides.h"

namespace Coco
{
    /*EnginePlatform::~EnginePlatform()
    {
        COCO_ASSERT(!_engine, "Engine was not null during destructor of EnginePlatform. Did you forget to call Shutdown()?");
    }

    Engine* EnginePlatform::CreateEngine()
    {
        auto* engine = New<Engine>(*GetDefaultAllocator(), this);
        _engine.reset(engine);
        Initialize();

        return _engine.get();
    }

    void EnginePlatform::Shutdown()
    {
        if (_engine)
            _engine.reset();
    }

    EnginePlatform::EnginePlatform() :
        _memoryManager(this),
        _engine()
    {}*/
}
