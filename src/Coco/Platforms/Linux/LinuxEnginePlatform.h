//
// Created by cullen on 2/24/26.
//

#ifndef COCOENGINE_LINUXENGINEPLATFORM_H
#define COCOENGINE_LINUXENGINEPLATFORM_H
#include "Coco/Core/EnginePlatform.h"
#include "Coco/Core/Memory/Allocators/HeapAllocator.h"
#include "Coco/Core/ProcessLoop/TickListener.h"

#ifdef COCO_SERVICE_RENDERING
#include <Coco/Rendering/RenderingEnginePlatform.h>
#endif

#ifdef COCO_SERVICE_WINDOWING
#include <Coco/Windowing/WindowingEnginePlatform.h>
#include "Windowing/LinuxWindowSystem.h"
#endif

#ifdef COCO_SERVICE_INPUT
#include <Coco/Input/InputEnginePlatform.h>
#endif

namespace Coco
{
    /// @brief The implementation of EnginePlatform for Linux platforms
    class LinuxEnginePlatform :
        public EnginePlatform
#ifdef COCO_SERVICE_RENDERING
        , public RenderingEnginePlatform
#endif
#ifdef COCO_SERVICE_WINDOWING
        , public WindowingEnginePlatform
#endif
#ifdef COCO_SERVICE_INPUT
        , public InputEnginePlatform
#endif
    {
    public:
        LinuxEnginePlatform();
        ~LinuxEnginePlatform() noexcept override;

        MemoryManager* GetMemoryManager() override { return &_memoryManager; };
        const MemoryManager* GetMemoryManager() const override { return &_memoryManager; }
        void* AllocateMemory(uint8 group, uint64 size) override;
        void FreeMemory(uint8 group, void* memory, uint64 size) noexcept override;
        Allocator* GetDefaultAllocator() override { return &_defaultAllocator; }
        TimeSpan GetCurrentTime() const override;
        TimeSpan GetRunningTime() const override { return GetCurrentTime() - _startTime;}
        DateTime GetLocalTime() const override;
        DateTime GetUtcTime() const override;
        void Sleep(const TimeSpan& sleepTime) const override;
        Engine* CreateEngine() override;
        void Shutdown() override;

    private:
        MemoryManager _memoryManager;
        UniquePtr<Engine> _engine;
        HeapAllocator _defaultAllocator;
        TimeSpan _startTime;

#ifdef  COCO_SERVICE_RENDERING
    public:
        void GetRenderingExtensions(GraphicsPlatform& graphicsPlatform, PlatformRenderingExtensions& extensions) override;
#endif

#ifdef COCO_SERVICE_WINDOWING
    public:
        /// @brief The tick order for processing events from the operating system
        static constexpr int ProcessEventsTickOrder = -10000;

        uint16 GetDisplayCount() const override;
        void GetDisplays(Span<DisplayInfo> outDisplays) override;
        Ref<Window> CreatePlatformWindow(const WindowCreateParams& createParams) override;
        uint64 GetWindowCount(bool includeInactive) const override;
        void GetWindows(bool includeInactive, Span<Ref<Window>> outWindows) override;
        void DestroyWindows() override;

    private:
        TickListener _processEventsTickListener;
        UniquePtr<LinuxWindowSystem> _windowSystem;

    private:
        /// @brief Tick handler for processing operating system events
        /// @param tickInfo The tick info
        void ProcessEventsTick(const TickInfo& tickInfo);
#endif

#ifdef COCO_SERVICE_INPUT
    public:
        bool SupportsMouse() const override;
        bool SupportsKeyboard() const override;
        bool SupportsRawInput() const override;
#endif
    };
} // Coco::Linux

#endif //COCOENGINE_LINUXENGINEPLATFORM_H
