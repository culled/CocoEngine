#pragma once

#include <Coco/Core/Core.h>
#include "Query.h"

namespace Coco
{
	/// <summary>
	/// An event that can fire to listening handlers
	/// </summary>
	/// <typeparam name="...Args">The argument types for the event</typeparam>
	template<typename ... Args>
	class COCOAPI Event : public Query<bool, Args...>
	{
	public:
		using HandlerFunctionType = Query<bool, Args...>::HandlerFunctionType;
		using HandlerType = Query<bool, Args...>::HandlerType;
		using HandlerID = Query<bool, Args...>::HandlerID;

		/// <summary>
		/// Invokes this event and sets the given value to true if a handler handled the event
		/// </summary>
		/// <param name="value">The output handled value</param>
		/// <param name="...params">The parameters for the event</param>
		virtual bool Invoke(bool* value, Args... params) override
		{
			List<Ref<HandlerType>> handlersCopy = this->GetHandlerListCopy();

			bool handled = false;

			for (const Ref<HandlerType>& handler : handlersCopy)
			{
				(*handler)(&handled, params...);

				if (handled)
					break;
			}

			if (value != nullptr)
				*value = handled;

			return handled;
		}

		/// <summary>
		/// Invokes this event with the given parameters
		/// </summary>
		/// <param name="...params">The parameters for the event</param>
		void InvokeEvent(Args... params)
		{
			Invoke(nullptr, std::forward<Args>(params)...);
		}

		void operator()(Args... params)
		{
			InvokeEvent(std::forward<Args>(params)...);
		}
	};
}

