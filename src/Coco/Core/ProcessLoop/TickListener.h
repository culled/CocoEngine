//
// Created by cullen on 2/25/26.
//

#ifndef COCOENGINE_TICKLISTENER_H
#define COCOENGINE_TICKLISTENER_H
#include <functional>

#include "TickInfo.h"
#include "Coco/Core/Asserts.h"

namespace Coco
{
    class ProcessLoop;

    /// @brief Listens and dispatches ticks from a ProcessLoop
    class TickListener
    {
    public:
        using CallbackFunc = std::function<void(const TickInfo&)>;

    public:
        TickListener(CallbackFunc callbackFunction, int order, bool enabled = true);

        template<typename InstanceType>
        TickListener(InstanceType* instance, void(InstanceType::* callbackFunction)(const TickInfo&), int order, bool enabled = true) :
        TickListener(nullptr, order, enabled)
        {
            SetCallbackFunction(instance, callbackFunction);
        }

        TickListener(TickListener&& other) noexcept;
        ~TickListener() noexcept;

        TickListener& operator=(TickListener&& rhs) noexcept;

        TickListener(const TickListener&) = delete;
        TickListener& operator=(const TickListener&) = delete;

        friend void swap(TickListener& a, TickListener& b) noexcept;

        /// @brief Sets the tick callback function to a member function of a class instance
        /// @tparam InstanceType The class type
        /// @param instance A pointer to the instance
        /// @param callbackFunction The member function that will be invoked during a tick
        template<typename InstanceType>
        void SetCallbackFunction(InstanceType* instance, void(InstanceType::* callbackFunction)(const TickInfo&))
        {
            COCO_ASSERT(instance, "Instance was null");
            COCO_ASSERT(callbackFunction, "Callback was null");

            _callback = [instance, callbackFunction](const TickInfo& info) { return (instance->*callbackFunction)(info); };
        }

        /// @brief Sets the tick callback function
        /// @param callbackFunction The function that will be invoked during a tick
        void SetCallbackFunction(const CallbackFunc& callbackFunction);

        /// @brief Starts listening to ticks from the given ProcessLoop
        /// @param loop The loop to listen to
        void ListenTo(ProcessLoop& loop);

        /// @brief Stops listening to ticks
        void StopListening() noexcept;

        /// @brief Dispatches a tick to the callback function
        /// @param tickInfo The tick info
        void DispatchTick(const TickInfo& tickInfo) const;

        /// @brief Sets the enabled state.
        /// This can be used to temporarily disable dispatching ticks to the callback function without disconnecting from the ProcessLoop
        /// @param enabled The enabled state
        void SetEnabled(bool enabled);

        /// @brief Gets the enabled state
        /// @return The enabled state
        bool IsEnabled() const noexcept { return _isEnabled; }


        /// @brief Gets the order in the listener stack that this listener will be notified by.
        /// TickListeners are notified in ascending order
        /// @return The order
        int GetOrder() const noexcept { return _order; }

    private:
        ProcessLoop* _connectedLoop;
        CallbackFunc _callback;
        int _order;
        bool _isEnabled;
    };
} // Coco

#endif //COCOENGINE_TICKLISTENER_H