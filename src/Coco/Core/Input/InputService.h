#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Services/EngineService.h>
#include <Coco/Core/MainLoop/MainLoopTickListener.h>

namespace Coco::Input
{
    class Keyboard;
    class Mouse;

    class COCOAPI InputService : public EngineService
    {
    private:
        Managed<Keyboard> _keyboard;
        Managed<Mouse> _mouse;
        Ref<MainLoopTickListener> _tickListener;

    public:
        const static int InputTickPriority = 1000;

        InputService();
        virtual ~InputService() override;

        virtual Logging::Logger* GetLogger() const;
        virtual void Start() override;

        Keyboard* GetKeyboard() const { return _keyboard.get(); }
        Mouse* GetMouse() const { return _mouse.get(); }

    private:
        void Tick(double deltaTime);
    };
}

