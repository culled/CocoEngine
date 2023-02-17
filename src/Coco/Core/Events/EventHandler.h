#pragma once

#include <Coco/Core/Core.h>
#include "EventArgs.h"

#include <functional>
#include <atomic>

namespace Coco
{
	/// <summary>
	/// A handler for events
	/// </summary>
	/// <typeparam name="...Args">The argument types for the event</typeparam>
	template<typename ... Args>
	class COCOAPI EventHandler
	{
	public:
		using HandlerFunctionType = std::function<bool(Args...)>;

	private:
		static std::atomic_uint _handlerIdCounter;

		HandlerFunctionType _handler;
		unsigned int _id;

	public:
		EventHandler(const HandlerFunctionType& handlerFunction) : _handler(handlerFunction)
		{
			_id = ++_handlerIdCounter;
		}

		virtual ~EventHandler() = default;

		bool operator()(Args ... params) const
		{
			if (_handler)
				return _handler(params...);

			return false;
		}

		virtual bool operator==(EventHandler* other) const { return _id == other->_id; }

		/// <summary>
		/// Gets this handler's ID
		/// </summary>
		/// <returns>This handler's ID</returns>
		unsigned int GetID() const { return _id; }
	};

	template <typename... Args>
	std::atomic_uint EventHandler<Args...>::_handlerIdCounter(0);

	/// <summary>
	/// Handler for events that use an instance and member function
	/// </summary>
	/// <typeparam name="T">The type of instance</typeparam>
	/// <typeparam name="...Args">The argument types for the event</typeparam>
	template<typename T, typename ... Args>
	class COCOAPI ObjectEventHandler : public EventHandler<Args...>
	{
		using ObjectHandlerFunctionType = bool(T::*)(Args...);

	private:
		T* _instance;
		ObjectHandlerFunctionType _function;

	public:
		ObjectEventHandler(T* instance, ObjectHandlerFunctionType function) : 
			EventHandler<Args...>(std::bind(function, instance, std::placeholders::_1)),
			_instance(instance),
			_function(function)
		{}

		virtual ~ObjectEventHandler() override = default;

		virtual bool operator==(EventHandler<Args...>* other) const override
		{
			if (ObjectEventHandler<T, Args...>* otherPtr = dynamic_cast<ObjectEventHandler<T, Args...>*>(other))
			{
				return _instance == otherPtr->_instance &&
					_function == otherPtr->_function;
			}

			return false;
		}
	};
}