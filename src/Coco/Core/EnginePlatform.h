//
// Created by cullen on 2/24/26.
//

#ifndef COCOENGINE_ENGINEPLATFORM_H
#define COCOENGINE_ENGINEPLATFORM_H
#include "Memory/MemoryManager.h"
#include "Memory/Allocator.h"
#include "Memory/Ptrs.h"
#include "Types/DateTime.h"
#include "Types/TimeSpan.h"

namespace Coco
{
    class Engine;

    class EnginePlatform
    {
    public:
        virtual ~EnginePlatform() = default;

        virtual void* AllocateMemory(uint8 group, uint64 size) = 0;
        virtual void FreeMemory(uint8 group, void* memory, uint64 size) noexcept = 0;
        virtual Allocator* GetDefaultAllocator() = 0;
        virtual TimeSpan GetCurrentTime() const = 0;
        virtual TimeSpan GetRunningTime() const = 0;
        virtual DateTime GetLocalTime() const = 0;
        virtual DateTime GetUtcTime() const = 0;
        virtual void Sleep(const TimeSpan& sleepTime) const = 0;

        virtual MemoryManager* GetMemoryManager() = 0;
        virtual const MemoryManager* GetMemoryManager() const = 0;
        virtual Engine* CreateEngine() = 0;
        virtual void Shutdown() = 0;

        /*MemoryManager* GetMemoryManager() { return &_memoryManager; };
        const MemoryManager* GetMemoryManager() const { return &_memoryManager; }

        Engine* CreateEngine();

        virtual void Shutdown();

    protected:
        EnginePlatform();

        virtual void Initialize() = 0;

    protected:
        MemoryManager _memoryManager;
        UniquePtr<Engine> _engine;*/
    };
}
#endif //COCOENGINE_ENGINEPLATFORM_H
