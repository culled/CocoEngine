#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Services/EngineService.h>
#include <Coco/Core/MainLoop/MainLoopTickListener.h>

#include "Keyboard.h"
#include "Mouse.h"

namespace Coco::Input
{
    /// <summary>
    /// A service that handles input from peripherals
    /// </summary>
    class COCOAPI InputService : public EngineService
    {
    public:
        const static int ProcessTickPriority = -10000;
        const static int LateProcessTickPriority = 10000;

    private:
        Managed<Keyboard> _keyboard;
        Managed<Mouse> _mouse;
        Ref<MainLoopTickListener> _processListener;
        Ref<MainLoopTickListener> _lateProcessListener;

    public:
        InputService();
        virtual ~InputService() override;

        InputService(const InputService&) = delete;
        InputService(InputService&&) = delete;

        InputService& operator=(const InputService&) = delete;
        InputService& operator=(InputService&&) = delete;

        Logging::Logger* GetLogger() const noexcept override;
        void Start() override;

        /// <summary>
        /// Gets the current keyboard
        /// </summary>
        /// <returns>The current keyboard</returns>
        Keyboard* GetKeyboard() const noexcept { return _keyboard.get(); }

        /// <summary>
        /// Gets the current mouse
        /// </summary>
        /// <returns>The current mouse</returns>
        Mouse* GetMouse() const noexcept { return _mouse.get(); }

    private:
        /// <summary>
        /// Tick for updating the current state of the peripherals
        /// </summary>
        /// <param name="deltaTime">The number of seconds since the last tick</param>
        void Process(double deltaTime);

        /// <summary>
        /// Tick for updating the previous state of the peripherals
        /// </summary>
        /// <param name="deltaTime">The number of seconds since the last tick</param>
        void LateProcess(double deltaTime) noexcept;
    };
}

