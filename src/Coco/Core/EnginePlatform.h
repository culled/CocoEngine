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

    /// @brief Base class for the platform that links the Engine to the operating system
    class EnginePlatform
    {
    public:
        virtual ~EnginePlatform() = default;

        /// @brief Allocates a block of memory from the device
        /// @param group The group tag for the memory
        /// @param size The number of bytes requested
        /// @return A pointer to the first byte in the block of allocated memory
        virtual void* AllocateMemory(uint8 group, uint64 size) = 0;

        /// @brief Frees a block of memory
        /// @param group The group tag for the memory
        /// @param memory A pointer to the first byte in the block of memory
        /// @param size The number of bytes being freed. This should match the number of bytes originally allocated
        virtual void FreeMemory(uint8 group, void* memory, uint64 size) noexcept = 0;

        /// @brief Gets the default allocator for the platform. This usually is some sort of heap allocator
        /// @return The default allocator
        virtual Allocator* GetDefaultAllocator() = 0;

        /// @brief Gets the time that the system has been running
        /// @return The time that the system has been running
        virtual TimeSpan GetCurrentTime() const = 0;

        /// @brief Gets the time that the engine has been running
        /// @return The amount of time the engine has been running
        virtual TimeSpan GetRunningTime() const = 0;

        /// @brief Gets the local time of the system
        /// @return The local time of the system
        virtual DateTime GetLocalTime() const = 0;

        /// @brief Gets the time of the system in UTC time
        /// @return The time of the system in UTC time
        virtual DateTime GetUtcTime() const = 0;

        /// @brief Allows the calling thread to sleep for at least the given amount of time. The thread will not be woken up after the exact time has passed, and instead will be woken up at some point after the given amount of time has passed
        /// @param sleepTime The minimum amount of time to sleep
        virtual void Sleep(const TimeSpan& sleepTime) const = 0;

        /// @brief Gets the memory manager
        /// @return The memory manager
        virtual MemoryManager* GetMemoryManager() = 0;

        /// @brief Gets the memory manager
        /// @return The memory manager
        virtual const MemoryManager* GetMemoryManager() const = 0;

        /// @brief Creates the engine instance
        /// @return The engine instance
        virtual Engine* CreateEngine() = 0;

        /// @brief Shuts down and destroys the engine instance
        virtual void Shutdown() = 0;
    };
}
#endif //COCOENGINE_ENGINEPLATFORM_H
