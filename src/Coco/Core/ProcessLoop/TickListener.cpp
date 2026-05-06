//
// Created by cullen on 2/25/26.
//

#include "TickListener.h"
#include "ProcessLoop.h"

namespace Coco
{
    TickListener::TickListener(CallbackFunc callbackFunction, int order, bool enabled) :
        _connectedLoop(nullptr),
        _callback(std::move(callbackFunction)),
        _order(order),
        _isEnabled(enabled)
    {}

    TickListener::TickListener(TickListener&& other) noexcept
    : TickListener(nullptr, 0)
    {
        swap(*this, other);
    }

    TickListener::~TickListener() noexcept
    {
        StopListening();
    }

    TickListener& TickListener::operator=(TickListener&& rhs) noexcept
    {
        swap(*this, rhs);
        return *this;
    }

    void TickListener::SetCallbackFunction(const CallbackFunc& callbackFunction)
    {
        _callback = callbackFunction;
    }

    void TickListener::ListenTo(ProcessLoop& loop)
    {
        if (&loop == _connectedLoop)
            return;

        if (_connectedLoop)
            StopListening();

        loop.AddTickListener(*this);
        _connectedLoop = &loop;
    }

    void TickListener::StopListening() noexcept
    {
        if (!_connectedLoop)
            return;

        try
        {
            _connectedLoop->RemoveTickListener(*this);
            _connectedLoop = nullptr;
        }
        catch (...)
        {
            COCO_ASSERT(false, "Failed to stop listening to ProcessLoop");
        }
    }

    void TickListener::DispatchTick(const TickInfo& tickInfo) const
    {
        if (!_callback || !_isEnabled)
            return;

        _callback(tickInfo);
    }

    void TickListener::SetEnabled(bool enabled)
    {
        _isEnabled = enabled;
    }

    void swap(TickListener& a, TickListener& b) noexcept
    {
        using std::swap;

        swap(a._callback, b._callback);
        swap(a._order, b._order);
        swap(a._isEnabled, b._isEnabled);

        ProcessLoop* aLoop = a._connectedLoop;
        ProcessLoop* bLoop = b._connectedLoop;

        a.StopListening();
        b.StopListening();

        if (aLoop)
        {
            try
            {
                b.ListenTo(*aLoop);
            }
            catch (...) {}
        }

        if (bLoop)
        {
            try
            {
                a.ListenTo(*bLoop);
            }
            catch (...) {}
        }
    }
} // Coco