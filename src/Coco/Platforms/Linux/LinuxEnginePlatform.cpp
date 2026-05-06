//
// Created by cullen on 2/24/26.
//

#include "LinuxEnginePlatform.h"

#ifdef COCO_SERVICE_WINDOWING
#include "Windowing/X11/X11WindowSystem.h"
#endif

#include "Coco/Core/Engine.h"

namespace Coco
{
    LinuxEnginePlatform::LinuxEnginePlatform() :
        _memoryManager(*this),
        _engine(),
        _defaultAllocator(0),
        _startTime()
    #ifdef COCO_SERVICE_WINDOWING
        , _processEventsTickListener(this, &LinuxEnginePlatform::ProcessEventsTick, ProcessEventsTickOrder)
        , _windowSystem()
    #endif
    {
        _startTime = LinuxEnginePlatform::GetCurrentTime();
    }

    LinuxEnginePlatform::~LinuxEnginePlatform() noexcept
    {}

    void* LinuxEnginePlatform::AllocateMemory(uint8 group, uint64 size)
    {
        void* memory = malloc(size);
        _memoryManager.AllocationMade(group, size);
        return memory;
    }

    void LinuxEnginePlatform::FreeMemory(uint8 group, void* memory, uint64 size) noexcept
    {
        free(memory);
        _memoryManager.AllocationFreed(group, size);
    }

    TimeSpan LinuxEnginePlatform::GetCurrentTime() const
    {
        timespec time{};
        clock_gettime(CLOCK_MONOTONIC, &time);

        uint64 microseconds = time.tv_sec * TimeSpan::MicroSecsPerSecond + time.tv_nsec / 1000;
        return TimeSpan(microseconds);
    }

    DateTime LinuxEnginePlatform::GetLocalTime() const
    {
        DateTime utcTime = GetUtcTime();
        time_t utcSeconds = static_cast<time_t>(utcTime.GetTotalSeconds());
        tm* local = localtime(&utcSeconds);
        time_t localSeconds = utcSeconds + local->tm_gmtoff;
        return DateTime(localSeconds * DateTime::MSecsPerSecond + utcTime.GetMillisecond());
        //return DateTime(local->tm_year + 1900, local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec, utcTime.GetMillisecond());
    }

    DateTime LinuxEnginePlatform::GetUtcTime() const
    {
        timespec time{};
        clock_gettime(CLOCK_REALTIME, &time);

        int64 epochMillis = (time.tv_sec * 1000) + (time.tv_nsec / (1000 * 1000));

        return DateTime(epochMillis);
    }

    void LinuxEnginePlatform::Sleep(const TimeSpan& sleepTime) const
    {
        usleep(sleepTime.Microseconds);
    }

    Engine* LinuxEnginePlatform::CreateEngine()
    {
        auto* engine = New<Engine>(*GetDefaultAllocator(), this);
        _engine.reset(engine);

        #ifdef COCO_SERVICE_WINDOWING
        // TODO: Wayland window system with env variable to switch
        _windowSystem = CreateDefaultUnique<X11WindowSystem>(_engine.get());
        _processEventsTickListener.ListenTo(*_engine->GetMainLoop());
        #endif

        return _engine.get();
    }

    void LinuxEnginePlatform::Shutdown()
    {
        _engine->Shutdown();

        #ifdef COCO_SERVICE_WINDOWING
        _windowSystem.reset();
        #endif

        _engine.reset();
    }

    /*void LinuxEnginePlatform::Initialize()
    {
    #ifdef COCO_SERVICE_WINDOWING
        // TODO: Wayland window system with env variable to switch
        _windowSystem = CreateDefaultUnique<X11WindowSystem>();
        _processEventsTickListener.ListenTo(*_engine->GetMainLoop());
        #endif
    }*/

#ifdef COCO_SERVICE_RENDERING
    void LinuxEnginePlatform::GetRenderingExtensions(GraphicsPlatform& graphicsPlatform, PlatformRenderingExtensions& extensions)
    {
#ifdef COCO_SERVICE_WINDOWING
        _windowSystem->GetRenderingExtensions(graphicsPlatform, extensions);
#endif
    }
#endif

#ifdef COCO_SERVICE_WINDOWING
    uint16 LinuxEnginePlatform::GetDisplayCount() const
    {
        return _windowSystem->GetDisplayCount();
    }

    void LinuxEnginePlatform::GetDisplays(Span<DisplayInfo> outDisplays)
    {
        _windowSystem->GetDisplays(outDisplays);
    }

    Ref<Window> LinuxEnginePlatform::CreatePlatformWindow(const WindowCreateParams& createParams)
    {
        return _windowSystem->CreateWindow(createParams);
    }

    uint64 LinuxEnginePlatform::GetWindowCount(bool includeInactive) const
    {
        return _windowSystem->GetWindowCount(includeInactive);
    }

    void LinuxEnginePlatform::GetWindows(bool includeInactive, Span<Ref<Window>> outWindows)
    {
        _windowSystem->GetWindows(includeInactive, outWindows);
    }

    void LinuxEnginePlatform::DestroyWindows()
    {
        _windowSystem->DestroyWindows();
    }

    void LinuxEnginePlatform::ProcessEventsTick(const TickInfo& tickInfo)
    {
        _windowSystem->ProcessEvents();
    }
#endif

    #ifdef COCO_SERVICE_INPUT
    bool LinuxEnginePlatform::SupportsMouse() const
    {
        #ifdef COCO_SERVICE_WINDOWING
        return _windowSystem->SupportsMouse();
        #else
        return false;
        #endif
    }

    bool LinuxEnginePlatform::SupportsKeyboard() const
    {
        #ifdef COCO_SERVICE_WINDOWING
        return _windowSystem->SupportsKeyboard();
        #else
        return false;
        #endif
    }

    bool LinuxEnginePlatform::SupportsRawInput() const
    {
        #ifdef COCO_SERVICE_WINDOWING
        return _windowSystem->SupportsRawInput();
        #else
        return false;
        #endif
    }
    #endif

    /*bool LinuxEnginePlatform::CheckTerminalColorSupport() const
    {
        // If "NO_COLOR" is set, then don't display color
        if (getenv("NO_COLOR") != nullptr)
            return false;

        // Get the type of terminal
        const char* term = getenv("TERM");

        if (!term || strcmp(term, "dumb") == 0)
            return false;

        return true;
    }*/
} // Coco
