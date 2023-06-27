#pragma once

#include <Coco/Core/Services/EngineService.h>
#include <Coco/Core/MainLoop/MainLoopTickListener.h>

#include "Keyboard.h"
#include "Mouse.h"

namespace Coco::Input
{
    /// @brief A service that handles input from peripherals
    class COCOAPI InputService : public EngineService
    {
    public:
        /// @brief Priority for the tick handling input processing
        const static int ProcessTickPriority = -10000;

        /// @brief Priority for the tick handling late input processing
        const static int LateProcessTickPriority = 10000;

    private:
        ManagedRef<Keyboard> _keyboard;
        ManagedRef<Mouse> _mouse;

    public:
        InputService(EngineServiceManager* serviceManager);
        virtual ~InputService() override;

        InputService(const InputService&) = delete;
        InputService(InputService&&) = delete;

        InputService& operator=(const InputService&) = delete;
        InputService& operator=(InputService&&) = delete;

        /// @brief Gets the current keyboard
        /// @return The current keyboard
        Keyboard* GetKeyboard() noexcept { return _keyboard.Get(); }

        /// @brief Gets the current mouse
        /// @return The current mouse
        Mouse* GetMouse() noexcept { return _mouse.Get(); }

    private:
        /// @brief Tick for updating the current state of the peripherals
        /// @param deltaTime The number of seconds since the last tick
        void Process(double deltaTime);

        /// @brief Tick for updating the previous state of the peripherals
       /// @param deltaTime The number of seconds since the last tick
        void LateProcess(double deltaTime) noexcept;
    };
}

