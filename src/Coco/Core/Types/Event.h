//
// Created by cullen on 3/2/26.
//

#ifndef COCOENGINE_EVENT_H
#define COCOENGINE_EVENT_H
#include "Coco/Core/Memory/Allocator.h"
#include "Array.h"

namespace Coco
{
    template<typename ... Args>
    class Event;

    template<typename ... Args>
    class EventListener
    {
        friend class Event<Args...>;

    public:
        using CallbackFunc = std::function<bool(Args...)>;

    public:
        EventListener(const CallbackFunc& callbackFunc) :
            _callbackFunc(callbackFunc),
            _connectedEvent(nullptr)
        {}

        template<typename ObjectType>
        EventListener(ObjectType* instance, bool(ObjectType::* callbackFunction)(Args...)) :
            EventListener([instance, callbackFunction](Args&&... args) { return (instance->*callbackFunction)(std::forward<Args>(args)...); })
        {}

        ~EventListener() noexcept
        {
            if (_connectedEvent)
            {
                try
                {
                    _connectedEvent->Disconnect(*this);
                }
                catch (...) {}
            }
        }

    private:
        CallbackFunc _callbackFunc;
        Event<Args...>* _connectedEvent;
    };

    template<typename ... Args>
    class Event
    {
        template<typename...>
        friend class EventListener;

    public:
        Event(Allocator* allocator = nullptr) :
            _listeners(allocator),
            _currentIndex(0)
        {}

        ~Event() noexcept
        {
            try
            {
                while (!_listeners.IsEmpty())
                {
                    Disconnect(*_listeners.Back());
                }
            }
            catch (...) {}
        }

        void Connect(EventListener<Args...>& listener)
        {
            _listeners.Append(&listener);
            listener._connectedEvent = this;
        }

        void Disconnect(EventListener<Args...>& listener)
        {
            //int64 index = _listeners.Find([listener = &listener](const EventListener<Args...>* other) { return other == listener; });
            int64 index = _listeners.Find(&listener);
            if (index == -1)
                return;

            _listeners.RemoveAt(index);
            listener._connectedEvent = nullptr;

            if (_currentIndex >= index)
                --_currentIndex;
        }

        bool Invoke(Args&&... args)
        {
            for (_currentIndex = 0; _currentIndex < static_cast<int64>(_listeners.GetCount()); ++_currentIndex)
            {
                const EventListener<Args...>* listener = _listeners[_currentIndex];
                if (!listener->_callbackFunc)
                    continue;

                if (listener->_callbackFunc(std::forward<Args>(args)...))
                    return true;
            }

            return false;
        }

    private:
        Array<EventListener<Args...>*> _listeners;
        int64 _currentIndex;
    };
} // Coco

#endif //COCOENGINE_EVENT_H